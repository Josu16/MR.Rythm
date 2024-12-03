#ifndef ABRSEQUENCER_H
#define ABRSEQUENCER_H

#include <arduino.h>
#include <IntervalTimer.h>
#include <vector>

#include "RotaryEncoder.h"
#include "MidiParser.h"


class ABRSequencer {
    private:
        IntervalTimer mainTimer;
        char name[20];
        volatile uint16_t bpm;
        uint8_t numerator;
        uint8_t denominator;
        volatile uint32_t currentTick = 0;
        uint16_t lastBpm;

        static const int totalTicks = 768; // caso ejemplo para 4/4 con 1 compás.
        Pattern pattern;

        // Configuraión de variables para la reproducción
        unsigned int patternLength;
        volatile bool isPlaying;

        // Controles

        long positionRe = 0; // Posición del rotary encoder
        RotaryEncoder bpmRe;

        int pinFw;

        volatile bool footswitchState;
        volatile bool footswitchChanged;
        volatile unsigned long lastDebounceTime;
        const unsigned long debounceDelay = 200;  // Tiempo de debounce en milisegundos


        // Indicadores
        const int playLed = 14;
        volatile bool playledState = false;     // Estado actual del LED
        volatile uint32_t playLedOffTick = 0;   // Tick en el que se apagará el LED

        volatile uint32_t *triangleX;

        // Función estática para el timer
        static void timerCallback();
        // Referencia estática a la instancia actual
        static ABRSequencer* instance;

        // footswitches
        static void fwISR();
        void handleFootswitchInterrupt();


    public:
        ABRSequencer(int pinARe, int pinbRe, int pinFw, long bpm, volatile uint32_t *triangleX);
        void beginSequencer();
        void onTimer();
        void updateTimerInterval(); // variable que probablememte sirva
        // long getBpm();
        long cheeckBpm();
        // Updatin all things
        void update(); // Método para manejar actualizaciones en el loop principal
        void initializePattern();
        uint32_t getCurrentTick();
        void updateTrianglePosition();
        void allNotesOff(uint8_t channel);
};

#endif