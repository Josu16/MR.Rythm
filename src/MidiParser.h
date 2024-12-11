#ifndef MIDIPARSER_H
#define MIDIPARSER_H

#include <Arduino.h>
#include <SdFat.h>

struct MidiEvent {
    uint8_t type;      // Tipo de evento: NOTE_ON o NOTE_OFF
    uint8_t note;      // Nota MIDI
    uint8_t velocity;  // Velocidad
    uint32_t tick;     // Tiempo absoluto en ticks
};
 
struct Pattern {
    char tackName[15] = {0};
    uint16_t tempo;
    uint8_t numerator;
    uint8_t denominator;
    uint8_t measures; // IMPORTANT: limitado a 255 compases.
    uint32_t totalTicks;
    uint32_t numEvents;
    MidiEvent events[1000];
};

struct MidiFile {
    String midiFile;     // Nombre completo del archivo
    String patternName;  // Nombre del patrón
};


class MidiParser {
    private:
        const uint8_t sequencerPPQN = 96;
        SdFat sd;
        FsFile midiFile;
        uint16_t resolutionFile;
        uint16_t numRealEvents;
        uint8_t lastStatusByte;

        const char* directoryPath = "/patterns"; // Ruta del directorio
        std::vector<MidiFile> &midiFiles;
        Pattern &currentPattern;
        unsigned int patternIndex = 1;

        int noteToChannel[128];

        void setupNoteToChannelMapping();
        uint32_t readVLQ();
        bool parseHeader();
        void parseTrack();
        void handleMetaEvent(uint8_t type, uint32_t length);
        void handleMidiEvent(uint8_t status, uint8_t note, uint8_t velocity, uint32_t currentTick);

    public:
        MidiParser(std::vector<MidiFile> &files, Pattern& currentPattern);
        bool parseFile(unsigned int ptrnIndex);
        uint16_t getNumEvents();
        void getAvailablePatterns();
};

#endif