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
    unsigned int totalVariants;
    MidiEvent events[5][1000];
    int eventsByVariant[5];
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
        unsigned int currentVariantIndex;
        unsigned int patternIndex = 1;

        int noteToChannel[128];

        void setupNoteToChannelMapping();
        uint32_t readVLQ();
        bool parseHeader();
        void parseTrack();
        void handleMetaEvent(uint8_t type, uint32_t length);
        void handleMidiEvent(uint8_t status, uint8_t note, uint8_t velocity, uint32_t currentTick);

        void parseFile(String fullPath);
    public:
        MidiParser(std::vector<MidiFile> &files, Pattern& currentPattern);
        uint16_t getNumEvents(unsigned int variant);
        void getAvailablePatterns();
        bool parsePattern(unsigned int ptrnIndex);
};

#endif