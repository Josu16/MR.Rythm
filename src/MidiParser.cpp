#include <MidiParser.h>
#include <SdFat.h>
#include <math.h>

MidiParser::MidiParser(std::vector<MidiFile> &files, Pattern& pattern)
:
    midiFiles(files),
    currentPattern(pattern)
{

    if (!sd.begin(SdioConfig()))
    {
        Serial.println("Error inicializando la tarjeta SD");
        return;
    }

    // if (!midiFile)
    // {
    //     Serial.println("Error abriendo archivo MIDI");
    // }
    // Serial.println("Archivo abierto correctamente");
    setupNoteToChannelMapping();

    Serial.println("Hola parser");
}

// Inicializar la tabla
void MidiParser::setupNoteToChannelMapping() {
    // Inicializar todas las notas como "sin canal asignado"
    for (int i = 0; i < 128; ++i) {
        noteToChannel[i] = -1; // -1 indica que la nota no está asignada
    }

    // Asignar notas al canal 9
    int channel9Notes[] = {35, 36, 37, 38, 39, 40, 41, 42, 44, 45, 46, 48, 49, 50, 51, 52, 53, 55, 56, 57};
    for (int note : channel9Notes) {
        noteToChannel[note] = 8; // CANAL 9
    }

    // Asignar notas al canal 10
    int channel10Notes[] = {54, 59, 60, 61, 62, 63, 64, 69, 70, 82, 84, 89, 90, 91, 92, 93, 94, 95, 96, 97};
    for (int note : channel10Notes) {
        noteToChannel[note] = 9; // CANAL 10
    }

    // Asignar notas al canal 11
    int channel11Notes[] = {58, 65, 66, 67, 68, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 83, 85, 86, 87};
    for (int note : channel11Notes) {
        noteToChannel[note] = 10; // CANAL 11
    }
}

void MidiParser::getAvailablePatterns() {

    SdFile dir; // Directorio

    if (!dir.open(directoryPath)) { // Abre el directorio
        Serial.println("No se pudo abrir el directorio /patterns.");
        return;
    }

    while (midiFile.openNext(&dir, O_READ)) {
        char fileName[30];
        midiFile.getName(fileName, sizeof(fileName));
        MidiFile fileInfo;

        // Verifica si la extensión es .mid (case insensitive)
        if ((strstr(fileName, ".mid") || strstr(fileName, ".MID")) && !strstr(fileName, "._")) {
            // midiFiles.push_back(String(fileName)); // Agrega al vector
            fileInfo.midiFile = fileName;
            // Encuentra el guion para extraer el nombre del patrón
            char* dashPosition = strchr(fileName, '-');
            if (dashPosition != nullptr) {
                // Avanza después del guion
                char* patternNameStart = dashPosition + 1;

                // Encuentra el punto antes de la extensión
                char* dotPosition = strchr(patternNameStart, '.');
                size_t nameLength = 14; // Longitud máxima para el nombre

                if (dotPosition != nullptr) {
                    // Calcula la longitud real del nombre antes del punto
                    nameLength = dotPosition - patternNameStart;
                }

                // Limita la longitud al máximo permitido
                nameLength = nameLength > 14 ? 14 : nameLength;

                // Extrae el nombre del patrón sin la extensión
                char patternName[15] = {0}; // Espacio para 14 caracteres + terminador nulo
                strncpy(patternName, patternNameStart, nameLength);

                // Agrega el nombre del patrón al vector
                fileInfo.patternName = String(patternName);
                midiFiles.push_back(fileInfo);
            }
        }

        midiFile.close(); // Cierra el archivo actual
    }
    dir.close(); // Cierra el directorio

    // Ordena los archivos alfabéticamente
    std::sort(midiFiles.begin(), midiFiles.end(), [](const MidiFile& a, const MidiFile& b) {
        return a.midiFile < b.midiFile; // Orden ascendente por nombre de archivo
    });

    // Imprime la lista ordenada
    Serial.println("Archivos MIDI en /patterns:");
    for (size_t i = 0; i < midiFiles.size(); ++i) {
        Serial.println(midiFiles[i].patternName);
        Serial.println(midiFiles[i].midiFile);
    }
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

    char header[4] = {0};
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
    char trackHeader[5] = {"\n"};
    midiFile.read(trackHeader, 4);

    Serial.println("-----------");
    Serial.println(trackHeader);
    Serial.println("-----------");
    if (strncmp(trackHeader, "MTrk", 4) != 0)
    {
        Serial.print("Track inválido: ");
        Serial.print(trackHeader[0]);
        Serial.print(trackHeader[1]);
        Serial.print(trackHeader[2]);
        Serial.println(trackHeader[3]);
        return;
    }
    else
    {
        Serial.println("Track correcto");
    }

    uint32_t trackSize = (midiFile.read() << 24) | (midiFile.read() << 16) |
                         (midiFile.read() << 8) | midiFile.read();
    uint32_t currentTick = 0;

    // trackSize = 328;

    Serial.print("Tamaño del chunk: ");
    Serial.println(trackSize);

    // approach por datagrama
    bool endOfTrack = false;
    while (!endOfTrack) {
        // comenzar con el primer metaevento/evento
        // leer delta time
        uint32_t deltaTime = readVLQ();
        // byte actual a evaluar
        uint8_t midiByte;
        // para controlar el tiempo absoluto
        currentTick += deltaTime;

        // DETERMINAMOS SI ES EVENTO O METAEVENTO
            // AQUI DEBE IR EL MECANISMO DE RESUMEN DE 
            // BYTE DE STATUS.
        
        uint8_t statusByte = midiFile.read();
        if (statusByte == 0xFF) { // Metaevent
            // uint32_t length = readVLQ();
            midiByte = midiFile.read();
            uint32_t length = midiFile.read();
            endOfTrack = handleMetaEvent(midiByte, length); // en este caso midiByte es un metaEventType
        } 
        else {
            uint8_t messageType = statusByte & 0xF0; // Extrae el tipo de mensaje (bits 4-7)
            switch (messageType) {
                case 0x80: // Note Off
                case 0x90: { // Note On
                    uint8_t midiByte = midiFile.read();
                    uint8_t velocity = midiFile.read();
                    handleMidiEvent(statusByte, midiByte, velocity, currentTick);
                    break;
                }
                case 0xA0: { // Aftertouch Polifónico
                    uint8_t note = midiFile.read();
                    uint8_t amount = midiFile.read();
                    // handleMidiEvent(messageType, channel, note, amount, currentTick);
                    break;
                }
                case 0xB0: { // Control Change
                    uint8_t controller = midiFile.read();
                    uint8_t value = midiFile.read();
                    // handleMidiEvent(messageType, channel, controller, value, currentTick);
                    Serial.println("Control change");
                    break;
                }
                case 0xC0: { // Program Change
                    uint8_t program = midiFile.read();
                    // handleMidiEvent(messageType, channel, program, 0, currentTick); // Usamos 0 como valor por defecto
                    Serial.println("Control change");
                    break;
                }
                case 0xD0: { // Aftertouch de Canal
                    uint8_t amount = midiFile.read();
                    // handleMidiEvent(messageType, channel, amount, 0, currentTick);
                    break;
                }
                case 0xE0: { // Pitch Bend Change
                    uint8_t lsb = midiFile.read();
                    uint8_t msb = midiFile.read();
                    uint16_t pitchBendValue = (msb << 7) | lsb;
                    // handleMidiEvent(messageType, channel, pitchBendValue, 0, currentTick);
                    Serial.println("Control change");
                    break;
                }
                default:
                    Serial.print("Mensaje MIDI desconocido o no implementado: ");
                    Serial.println(statusByte, HEX);
                    break;
            }
        }
    }

    Serial.println("---Operaciones de fin de pista---");
    Serial.print("Ultimo tick: ");
    float lastTick = (currentTick * sequencerPPQN) / resolutionFile;
    Serial.println(lastTick);
    uint8_t qn = ((uint8_t)round(lastTick/sequencerPPQN));
    currentPattern.measures = qn/currentPattern.numerator;
    currentPattern.totalTicks = qn * sequencerPPQN;
    Serial.print("Compaases: ");
    Serial.println(currentPattern.measures);
    Serial.print("Total ticks: ");
    Serial.println(currentPattern.totalTicks);
    // Guardar el nombre del patrón desde el archivo
    strncpy(currentPattern.tackName, midiFiles[patternIndex-1].patternName.c_str(), sizeof(currentPattern.tackName) - 1);
    currentPattern.tackName[sizeof(currentPattern.tackName) - 1] = '\0'; // Asegurar terminación nula
}

bool MidiParser::parsePattern(unsigned int ptrnIndex)
{
    numRealEvents = 0;
    currentVariantIndex = 0;
    bool valid = true;
    // VALIDACIÓN DE EXISTENCIA DE SECUENCIA EN EL ÍNDICE
    if (ptrnIndex > midiFiles.size()) {
        for (size_t i = 0; i < 15; i++) {
            currentPattern.tackName[i] = '\0';
        }
        currentPattern.tempo = 120;
        currentPattern.totalTicks = 384;

        valid = false; // no hay más archivos
    } else {
        this -> patternIndex = ptrnIndex;

        String fullPath = String(directoryPath) + "/" + midiFiles[ptrnIndex-1].midiFile;
        
        parseFile(fullPath);

        currentVariantIndex ++;
        // RECUPERAR VARIANTES DE PATRÓN
        SdFile dir;
        // construir path del directorio
        char subDirectory[50];
        strncpy(subDirectory, directoryPath, 10);
        char subDirBuffer[4]; // 3 dígitos + terminador nulo
        // Formateamos el número con ceros a la izquierda
        snprintf(subDirBuffer, sizeof(subDirBuffer), "%03d", ptrnIndex);
        strcat(subDirectory, "/");
        strcat(subDirectory, subDirBuffer);
        // Verificar si el directorio existe
        Serial.print("Directorio: ");
        Serial.println(subDirectory);
        if (!sd.exists(subDirectory) || !dir.open(subDirectory)) {
            Serial.println("El directorio no existe o No se pudo abrir el subdirectorio");
            valid = false;
        }
        else {
            while (midiFile.openNext(&dir, O_READ)) {
                char fileName[30];
                midiFile.getName(fileName, sizeof(fileName));
                if ((strstr(fileName, ".mid") || strstr(fileName, ".MID")) && !strstr(fileName, "._")) { 
                    numRealEvents = 0;
                    fullPath = String(subDirectory) + "/" + fileName;
                    parseFile(fullPath);
                    currentVariantIndex ++;
                }
            }
        }
        currentPattern.totalVariants = currentVariantIndex;
    }
    return valid;
}

void MidiParser::parseFile(String fullPath) {
    /*
    TODO: SOLUCIÓN TEMPORAL al problema de la lectura de archivos midi
    esta configuración debe desaparecer para dar paso a la lectura
    desde la memoria flash más rápida, por ahora los samples y 
    los archivos midi se leen al cambiar de patrón.
    */
    if (!sd.begin(SdioConfig()))
    {
        Serial.println("Error inicializando la tarjeta SD");
        return;
    }
    const char* filePath = fullPath.c_str(); // Convierte a const char* para SdFat
    Serial.println(filePath); // Imprime la ruta completa para depuración
    midiFile = sd.open(filePath, FILE_READ);
    if (!midiFile)
    {
        Serial.println("Error abriendo archivo MIDI Base");
        return;
    }

    if (parseHeader())
    {
        while (midiFile.available())
        {
            parseTrack();
            // break;
        }
    }
    Serial.print("Eventos totales:");
    Serial.println(numRealEvents);
    currentPattern.eventsByVariant[currentVariantIndex] = numRealEvents;
    midiFile.close();
}

bool MidiParser::handleMetaEvent(uint8_t type, uint32_t length)
{
    bool endOfTrack = false;
    switch (type)
    {
        case 0x51:
        { // Set Tempo
            uint32_t tempo = (midiFile.read() << 16) | (midiFile.read() << 8) | midiFile.read();
            uint16_t realTempo = 60000000/tempo; // TODO: se debe redondear no truncar.
            Serial.print("Tempo: ");
            Serial.println(realTempo);
            currentPattern.tempo = realTempo;
            break;
        }
        case 0x03:
        { // Track name (en logic es el nombre del pasaje midi no de la pista)
            uint8_t indexName = 0;
            // Limpieza del arreglo antes de copiar
            for (size_t i = 0; i < 15; i++) {
                currentPattern.tackName[i] = '\0';
            }
            while (indexName < 15 && indexName < length) {
                currentPattern.tackName[indexName] = midiFile.read();
                indexName ++;
            }
            if (length > 15) {
                midiFile.seek(midiFile.position() + (length - indexName));
            }
            // Serial.println("Bueno");
            // Serial.println(currentPattern.tackName);//la expresión debe ser un valor L modificable
            break;
        }
        case 0x58:
        { // Time signature
            Serial.println("estamos en time signature");
            uint16_t value;
            midiFile.read((uint8_t*)&value, 2);

            // Separar los bytes
            // Serial.print(value & 0xFF);
            // Serial.print("/");
            // Serial.println(1 << (value >> 8) & 0xFF);
            currentPattern.numerator = value & 0xFF;
            currentPattern.denominator = 1 << (value >> 8) & 0xFF;
            midiFile.seek(midiFile.position() + (length-2));  // Incrementa el puntero del archivo
            break;
        }
        case 0x2F:
        {
            if (length == 0) {
                endOfTrack = true;
                Serial.println("Fin de la pista detectado");
            } 
            break;
        }
        default:
        {
            Serial.println("metaevento por defecto");
            midiFile.seek(midiFile.position() + length); // Saltar datos no relevantes
            break;
        }
    }
    return endOfTrack;
}

void MidiParser::handleMidiEvent(uint8_t status, uint8_t note, uint8_t velocity, uint32_t currentTick)
{
    // Obtener el canal de destino usando la tabla de mapeo
    int targetChannel = noteToChannel[note];

    // Los 4 bits MSB (tipo de mensaje) se quedan iguales
    uint8_t messageType = status & 0xF0;

    // Los 4 bits LSB (canal) se reemplazan con el nuevo canal
    uint8_t newStatus = messageType | (targetChannel & 0x0F);

    MidiEvent event;
    event.type = newStatus; //(status & 0xF0);
    event.note = note;
    event.velocity = velocity;
    event.tick = (currentTick * sequencerPPQN) / resolutionFile;

    // Debug: Mostrar información del canal
    uint8_t midiChannel = (newStatus & 0x0F); // Convertir de rango 0-15 a 1-16

    Serial.print("Canal: ");
    Serial.print(midiChannel);
    Serial.print(", Nota: ");
    Serial.print(note);
    Serial.print(", Velocidad: ");
    Serial.print(velocity);
    Serial.print(", current tick: ");
    Serial.println(event.tick);
    currentPattern.events[currentVariantIndex][numRealEvents] = event;
    numRealEvents ++;
}

uint16_t MidiParser::getNumEvents(unsigned int variant) {
    return currentPattern.eventsByVariant[variant];
}
