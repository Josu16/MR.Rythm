#include "MidiManager.h"

MidiManager::MidiManager(HardwareSerial &serialPort, uint32_t baudRate)
    : midiPort(serialPort), baudRate(baudRate) {}

void MidiManager::begin() {
    midiPort.begin(baudRate);
}

void MidiManager::noteOn(byte canal, byte nota, byte velocidad) {
    midiPort.write(0x90 | ((canal - 1) & 0x0F));
    midiPort.write(nota & 0x7F);
    midiPort.write(velocidad & 0x7F);
}

void MidiManager::noteOff(byte canal, byte nota, byte velocidad) {
    midiPort.write(0x80 | ((canal - 1) & 0x0F));
    midiPort.write(nota & 0x7F);
    midiPort.write(velocidad & 0x7F);
}
