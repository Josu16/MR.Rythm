// RotaryEncoder.h
#ifndef ROTARYENCODER_H
#define ROTARYENCODER_H

// #define ENCODER_DO_NOT_USE_INTERRUPTS
#include <Encoder.h>

class RotaryEncoder {
public:
    RotaryEncoder(int pinA, int pinB, long &position, unsigned long debounceDelay = 5);
    void update();

private:
    Encoder encoder;
    long &position;  // Referencia a la variable de posición externa
    long lastPosition;
    unsigned long lastDebounceTime;
    unsigned long debounceDelay;
};

#endif // ROTARYENCODER_H
