#ifndef INTERNALFLASHFS_H
#define INTERNALFLASHFS_H

#include <Arduino.h>
#include <LittleFS.h>

#include "Pattern.h"

#define PROG_FLASH_SIZE 1024 * 1024 * 7 // Specify size to use of onboard Teensy Program Flash chip
/*
4 MB for all patterns:
aprox 250 patrones con 5 variantes
aprox 1000 patrones con 1 variante

y quedan libres aproximadamente 3.9 MB
monitorear la variable "free for files:7894020"
que se muestra después de la compilación.
actualmente indica que hay libres 7894020 
en donde caben perfectamente los 4mb del FS
*/

class InternalFlashFS {
    private:
        LittleFS_Program mr9Fs;
        uint32_t diskSize;
        File patternFile;

        void printDirectory(FS &fs);
        void printDirectory(File dir, int numSpaces);
        void printSpaces(int num);
        void printTime(const DateTimeFields tm);
        void printSongDetails(const Pattern& s);

    public:
        InternalFlashFS();
        bool savePattern(const Pattern &currentPattern);
        void getPattern(uint16_t numberPattern, Pattern &requiredPattern);
        void listInternalMemoryPatterns();
};

#endif