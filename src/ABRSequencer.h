#ifndef ABRSEQUENCER_H
#define ABRSEQUENCER_H

#include <arduino.h>
#include <IntervalTimer.h>
#include <vector>

#include "RotaryEncoder.h"


// Definimos los tipos de eventos MIDI
        const uint8_t NOTE_ON = 0x90;
        const uint8_t NOTE_OFF = 0x80;

        // Definimos las notas de los instrumentos MIDI (ej. GM Drums)
        const uint8_t KICK_DRUM = 36;
        const uint8_t SNARE_DRUM = 38;
        const uint8_t HI_HAT = 42;

// Estructura para secuencias de eventos MIDI
struct MidiEvent {
    uint8_t type;     // NOTE_ON o NOTE_OFF
    uint8_t note;     // Nota MIDI (ej. 36 = KICK_DRUM)
    uint8_t velocity; // Velocidad (ej. 100)
    int tick;
};

class ABRSequencer {
    private:
        IntervalTimer mainTimer;
        volatile uint32_t currentTick = 0;
        volatile uint16_t bpm;
        uint16_t lastBpm;

        //AQUÍ VA drumPattern
        // MidiEvent drumPattern[4];
        static const int totalTicks = 384; // caso ejemplo para 4/4 con 1 compás.
        std::vector<MidiEvent> eventList[totalTicks];

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
        const unsigned long debounceDelay = 50;  // Tiempo de debounce en milisegundos


        // Indicadores
        const int playLed = 14;
        volatile bool playledState = false;     // Estado actual del LED
        volatile uint32_t playLedOffTick = 0;   // Tick en el que se apagará el LED

        // Función estática para el timer
        static void timerCallback();
        // Referencia estática a la instancia actual
        static ABRSequencer* instance;

        // footswitches
        static void fwISR();
        void handleFootswitchInterrupt();


    public:
        ABRSequencer(int pinARe, int pinbRe, int pinFw, long bpm);
        void beginSequencer();
        void onTimer();
        void updateTimerInterval(); // variable que probablememte sirva
        // long getBpm();
        long cheeckBpm();
        // Updatin all things
        void update(); // Método para manejar actualizaciones en el loop principal
        void initializePattern();
};

#endif