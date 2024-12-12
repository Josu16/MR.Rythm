#ifndef CONTROL_H
#define CONTROL_H

#include <arduino.h>

#include "RotaryEncoder.h"

const int pinA1Re = 2; // TODO: cambiar todas las declaraciones de pines a defines
const int pinB1Re = 3;
const int pinFw = 41;

const int pinA2Re = 4;
const int pinB2Re = 5;
const int pinLoockTempo = 40;

const int pinA3Re = 6;
const int pinB3Re = 7;
const int pinLoockVariation = 39;

// switches

// TEMPORALES, ESTAS SE MOVERÁN PRONTO

class Control {
    private:
        // Rotary Encoders
        RotaryEncoder bpmRE;
        // long positionBpmRe = 0;

        RotaryEncoder ptrnRE;
        // long positionPtrnRE = 0;

        RotaryEncoder variantRE;
        // long positionVariantRE = 1;


        volatile bool footswitchState;
        volatile bool footswitchChanged;
        volatile unsigned long lastDebounceTime = 0;
        const unsigned long debounceDelay = 200;  // Tiempo de debounce en milisegundos

        volatile bool loockTempoTriggered;
        bool loockTempoState;
        volatile bool loockTempoChanged;
        unsigned long lastDebounceTimeTempo = 0;
        const unsigned long debounceDelayTempo = 400;  // Tiempo de debounce en milisegundos

        // Referencia estática a la instancia actual
        static Control* instance;

    public:
        Control(volatile long tempo, volatile long pattern);

        void refreshControls();

        // lectura de controles
        long readBpm();
        long readPtrn();
        long readVariant();
        bool checkForFootswitch();
        bool cheeckForLoockTempo();
        
        // Seteo de controles
        void setBpm(int newValue);
        void setPtrn(int newValue);
        void setVariant(int newValue);
        void setMaxVariant(int newValue);

        // MANEJO DE INTERRUPCIONES
        // footswitches
        static void fwISR();
        void handleFootswitchInterrupt();
        
        static void loockTempoISR();
        void handleLoockTempooInterrupt();
};

#endif