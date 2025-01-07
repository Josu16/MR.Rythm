#ifndef MIDIPARSER_H
#define MIDIPARSER_H

#include <Arduino.h>
#include <SdFat.h>

#include "Pattern.h"
#include "InternalFlashFS.h"

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
        Pattern currentPattern;
        Pattern &playingPattern;
        unsigned int currentVariantIndex;
        char currentPatternName[15];

        int noteToChannel[128];

        InternalFlashFS mr9Fs;

        // PRUEBAS
        int maxEventsPattern = 0;

        bool parsePattern(String filename);
        void setupNoteToChannelMapping();
        uint32_t readVLQ();
        bool parseHeader();
        void parseTrack();
        bool handleMetaEvent(uint8_t type, uint32_t length);
        void handleMidiEvent(uint8_t status, uint8_t note, uint8_t velocity, uint32_t currentTick);

        void parseFile(String fullPath);
    public:
        MidiParser(std::vector<MidiFile> &files, Pattern& currentPattern);
        void getAvailablePatterns();
        void loadPattern(uint8_t numberPattern, Pattern &requiredPattern);
};

#endif