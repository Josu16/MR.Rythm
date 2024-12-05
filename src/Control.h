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

    public:
        Control(volatile long tempo, volatile long pattern);

        // lectura de controles
        long readBpm();
        long readPtrn();
        
        // Seteo de controles
        void setBpm(int newValue);
        void setPtrn(int newValue);
};

#endif