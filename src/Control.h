#ifndef CONTROL_H
#define CONTROL_H

#include <arduino.h>

#include "RotaryEncoder.h"

const int pinA1Re = 2; // TODO: cambiar todas las declaraciones de pines a defines
const int pinB1Re = 3;
const int pinFw = 41;

const int pinA2Re = 4;
const int pinB2Re = 5;
// TEMPORALES, ESTAS SE MOVERÁN PRONTO

class Control {
    private:
        // Rotary Encoders
        RotaryEncoder bpmRE;
        long positionBpmRe = 0; // Posición del rotary encoder

        RotaryEncoder ptrnRE;
        long positionPtrnRE = 0;

        // switches
        int pinFw = 41;

        volatile bool footswitchState;
        volatile bool footswitchChanged;
        volatile unsigned long lastDebounceTime;
        const unsigned long debounceDelay = 200;  // Tiempo de debounce en milisegundos

        // Referencia estática a la instancia actual
        static Control* instance;

    public:
        Control(volatile long tempo, volatile long pattern);

        void refreshControls();

        // lectura de controles
        long readBpm();
        long readPtrn();
        bool checkForFootswitch();
        
        // Seteo de controles
        void setBpm(int newValue);
        void setPtrn(int newValue);

        // MANEJO DE INTERRUPCIONES
        // footswitches
        static void fwISR();
        void handleFootswitchInterrupt();
};

#endif