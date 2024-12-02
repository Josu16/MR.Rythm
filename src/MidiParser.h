#ifndef MIDIPARSER_H
#define MIDIPARSER_H

#include <Arduino.h>
#include <SdFat.h>

struct MidEvent {
    uint8_t type;      // Tipo de evento: NOTE_ON o NOTE_OFF
    uint8_t note;      // Nota MIDI
    uint8_t velocity;  // Velocidad
    uint32_t tick;     // Tiempo absoluto en ticks
};

class MidiParser {
    private:
        const uint8_t sequencerPPQN = 96;
        SdFat sd;                   // Manejador de la SD
        FsFile midiFile;
        std::vector<MidEvent> events;
        uint16_t resolutionFile;

        uint8_t lastStatusByte;

        uint32_t readVLQ();
        bool parseHeader();
        void parseTrack();
        void handleMetaEvent(uint8_t type, uint32_t length);
        void handleMidiEvent(uint8_t status, uint8_t note, uint8_t velocity, uint32_t currentTick);

    public:
        MidiParser(const char *filename);
        bool parseFile();
        const std::vector<MidEvent> &getEvents() const {return events; };
};

#endif