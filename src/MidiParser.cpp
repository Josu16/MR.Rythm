#include <MidiParser.h>
#include <SdFat.h>

MidiParser::MidiParser(const char *filename, std::vector<MidiEvent>& parentEvents): events(parentEvents)
{
    numRealEvents = 0;
    if (!sd.begin(SdioConfig()))
    {
        Serial.println("Error inicializando la tarjeta SD");
        return;
    }

    midiFile = sd.open(filename, FILE_READ);
    if (!midiFile)
    {
        Serial.println("Error abriendo archivo MIDI");
    }
    Serial.println("Archivo abierto correctamente");
}

uint32_t MidiParser::readVLQ()
{
    uint32_t value = 0;
    uint8_t byte;

    do
    {
        byte = midiFile.read(); // lee un byte y avanza el puntero al siguiente

        value = (value << 7) | (byte & 0x7F);

    } while (byte & 0x80); // MoreSignificativeByte siga siendo 1
    // Serial.println("Terminó de leer el vlq");
    // midiFile.close();

    return value;
}

uint8_t getVLQLength(uint32_t value)
{
    if (value < 0x80)
        return 1;
    else if (value < 0x4000)
        return 2;
    else if (value < 0x200000)
        return 3;
    else
        return 4;
}

bool MidiParser::parseHeader()
{
    /*
    0x4D, 0x54, 0x68, 0x64, // "MThd" (identificador del header)
    0x00, 0x00, 0x00, 0x06, // Tamaño del header (6 bytes)
    0x00, 0x01,             // Formato (1: múltiples tracks sincronizados)
    0x00, 0x01,             // Número de tracks (1)
    0x01, 0xE0              // Resolución (480 ticks por quarter note)
    */
    bool validHeader = true;

    char header[4];
    midiFile.read(header, 4);
    if (strncmp(header, "MThd", 4) != 0)
    {
        Serial.println("Cabecera de archivo MIDI inválida");
        validHeader = false;
    }
    uint32_t headerSize = (midiFile.read() << 24) | (midiFile.read() << 16) |
                          (midiFile.read() << 8) | midiFile.read();
    if (headerSize != 6)
    {
        Serial.println("Tamaño del header inesperado");
        validHeader = false;
    }
    // lectura del formato (2bytes)
    uint16_t format = (midiFile.read() << 8) | midiFile.read();
    uint16_t numTracks = (midiFile.read() << 8) | midiFile.read();
    resolutionFile = (midiFile.read() << 8) | midiFile.read();

    // Imprimir información del header
    Serial.print("Formato: ");
    Serial.println(format);
    Serial.print("Tracks: ");
    Serial.println(numTracks);
    Serial.print("Resolución: ");
    Serial.println(resolutionFile);

    return validHeader;
}

void MidiParser::parseTrack()
{
    char trackHeader[4];
    midiFile.read(trackHeader, 4);

    Serial.println("-----------");
    Serial.println(trackHeader);
    Serial.println("-----------");
    if (strncmp(trackHeader, "MTrk", 4) != 0)
    {
        Serial.print("Track inválido: ");
        return;
    }
    else
    {
        Serial.println("Track correcto");
    }

    uint32_t trackSize = (midiFile.read() << 24) | (midiFile.read() << 16) |
                         (midiFile.read() << 8) | midiFile.read();
    uint32_t bytesRead = 0;
    uint32_t currentTick = 0;

    // trackSize = 328;

    Serial.println("Tamaño del chunk: ");
    Serial.println(trackSize);

    while (bytesRead < trackSize)
    {
        uint32_t deltaTime = readVLQ();
        uint8_t midiByte;

        bytesRead += getVLQLength(deltaTime);
        currentTick += deltaTime;

        uint8_t statusByte = midiFile.read();
        bytesRead++;
        if ((statusByte & 0x80) == 0)
        { // si N0 está prendido el MSB procesa el mensaje, entonces llegó un mensaje resumido
            midiByte = statusByte;
            statusByte = lastStatusByte;
            // alreadyReaded = true;
        }
        else
        { // si está PRENDIDo, entonces lee el PRIMER byte de datos del mensaje
            midiByte = midiFile.read();
            bytesRead++;
        }

        if (statusByte == 0xFF)
        { // Metaevent
            uint32_t length = readVLQ();
            bytesRead += 1 + getVLQLength(length);
            handleMetaEvent(midiByte, length); // en este caso midiByte es un metaEventType
            bytesRead += length;
        }
        else
        {

            if ((statusByte & 0xF0) == 0x80 || (statusByte & 0xF0) == 0x90)
            { // Note On/Off
                uint8_t velocity = midiFile.read();
                bytesRead++;
                handleMidiEvent(statusByte, midiByte, velocity, currentTick);
            }
            else
            { // otro tipo de mensajes
                Serial.print("Midi ignorado ");
                Serial.println(statusByte, HEX);

                if ((statusByte & 0xF0) == 0xC0)
                { // Program change
                    Serial.print("Complemento del ignorado 1: ");
                    Serial.println(midiByte, HEX);
                }
                else if ((statusByte & 0xF0) == 0xB0)
                { // Control change
                    Serial.print("Complemento del ignorado 1: ");
                    Serial.println(midiByte, HEX);

                    midiByte = midiFile.read();
                    Serial.print("Complemento del ignorado 2: ");
                    Serial.println(midiByte, HEX);
                    bytesRead++;
                }
            }
        }
        lastStatusByte = statusByte;
    }
    Serial.println("Terminé de leer todos los mensajes midi del track (según chunk)");

    /*
        IR A LA BÚSQUEDA DEL FIN DE TRACK
        NOTA: esta versión es alpha, puede no trabajar bien con todas los .mid
        resulta que por algún motivo el tamaño del chunk indicado no coincide con el real
        por tanto sobran 3 bytes antes de llegar al end of track (0xFF, 0x2F, 0x00), a
        continuación se agrega un código extra para ir a buscar ese fin de track manualmente
        posteriormente se va a revisar por qué pasa esto y como resolverlo.
    */

    // ir a la búsqueda del EOT.
    uint8_t tmpByte;
    while (midiFile.available()) {
        tmpByte = midiFile.read();
        if (tmpByte == 0xFF && midiFile.available()) { // encontramos un metaevento que probablemente sea EOT
            if (midiFile.read() == 0x2F && midiFile.available()) { // segunda sospecha del fin de pista
                if ( midiFile.read() == 0x00) {
                    Serial.println("Oficialmente se acaba la pista");
                }
            }
        }
    }
}

bool MidiParser::parseFile()
{
    if (!midiFile)
    {
        Serial.println("no existía ya");
        return false;
    }

    if (parseHeader())
    {
        while (midiFile.available())
        {
            parseTrack();
        }
    }
    midiFile.close();
    return true;
}

void MidiParser::handleMetaEvent(uint8_t type, uint32_t length)
{
    Serial.println("Se procesó un metaevento");
    switch (type)
    {
    case 0x51:
    { // Set Tempo
        uint32_t tempo = (midiFile.read() << 16) | (midiFile.read() << 8) | midiFile.read();
        Serial.print("Tempo: ");
        Serial.println(tempo);
        break;
    }
    default:
        midiFile.seek(midiFile.position() + length); // Saltar datos no relevantes
        break;
    }
}

void MidiParser::handleMidiEvent(uint8_t status, uint8_t note, uint8_t velocity, uint32_t currentTick)
{
    MidiEvent event;
    event.type = (status & 0xF0);
    event.note = note;
    event.velocity = velocity;
    event.tick = (currentTick * sequencerPPQN) / resolutionFile;

    // Debug: Mostrar información del canal

    events.push_back(event);
    numRealEvents ++;
}

uint16_t MidiParser::getNumEvents() {
    return numRealEvents;
}
