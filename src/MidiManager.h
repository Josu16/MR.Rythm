// MidiManager.h

#ifndef MIDIMANAGER_H
#define MIDIMANAGER_H

#include <Arduino.h>

class MidiManager {
public:
    MidiManager(HardwareSerial &serialPort, uint32_t baudRate = 31250);
    void begin(); // Inicializa el puerto serial MIDI
    void noteOn(byte canal, byte nota, byte velocidad); // Activa una nota
    void noteOff(byte canal, byte nota, byte velocidad); // Desactiva una nota

private:
    HardwareSerial &midiPort;
    uint32_t baudRate;
};

#endif // MIDIMANAGER_H
