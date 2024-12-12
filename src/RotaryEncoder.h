// RotaryEncoder.h
#ifndef ROTARYENCODER_H
#define ROTARYENCODER_H

// #define ENCODER_DO_NOT_USE_INTERRUPTS
#include <Encoder.h>

class RotaryEncoder {
public:
    RotaryEncoder(int pinA, int pinB, volatile long position, int minPosition, int maxPosition, unsigned long debounceDelay = 5);
    long getPosition();
    void setPosition(int newValue);
    // long RotaryEncoder::getValue();

    void setMax(int newMaxPosition);

private:
    Encoder encoder;
    volatile long position;  // Referencia a la variable de posición externa
    long lastPosition;
    unsigned long lastDebounceTime;
    unsigned long debounceDelay;
    int minPosition;
    int maxPosition;
};

#endif // ROTARYENCODER_H
