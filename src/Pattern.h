// Pattern.h
#ifndef PATTERN_H
#define PATTERN_H

#include <Arduino.h>

struct MidiEvent {
    uint8_t type;      // Tipo de evento: NOTE_ON o NOTE_OFF
    uint8_t note;      // Nota MIDI
    uint8_t velocity;  // Velocidad
    uint32_t tick;     // Tiempo absoluto en ticks
};

struct Pattern {
    char tackName[15] = {0};
    uint16_t number;
    uint16_t tempo;
    uint8_t numerator;
    uint8_t denominator;
    uint8_t measures; // IMPORTANT: limitado a 255 compases.
    uint32_t totalTicks;
    uint32_t numEvents;
    unsigned int totalVariants;
    MidiEvent events[5][550];
    /*
    events puede llegar a pesar por variante con 1000 eventos 8kb
    es muchísimo más que los 400 bytes que el midi original, pero
    este formato permite ya no tener que procesar nada en tiempo real.
     */
    int eventsByVariant[5];
};

#endif
