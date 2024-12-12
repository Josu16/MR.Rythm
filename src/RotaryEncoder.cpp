// RotaryEncoder.cpp
#include "RotaryEncoder.h"
#include <Arduino.h>

RotaryEncoder::RotaryEncoder(int pinA, int pinB, volatile long position, int minPos, int maxPos,  unsigned long debounceDelay)
    : encoder(pinA, pinB), position(position), debounceDelay(debounceDelay) {
    lastPosition = encoder.read() / 4;
    lastDebounceTime = 0;
    setPosition(position);

    minPosition = minPos;
    maxPosition = maxPos;
}

void RotaryEncoder::setPosition(int newValue) {
    encoder.write(newValue*4);
}

long RotaryEncoder::getPosition() {
    long currentPosition = encoder.read() / 4;
    // Aplicar los límites
    if (currentPosition < minPosition) {
        currentPosition = minPosition;
        setPosition(minPosition); // Resetea el encoder a MIN_POSITION
    } 
    else if (currentPosition > maxPosition) {
        currentPosition = maxPosition;
        setPosition(maxPosition); // Resetea el encoder a MAX_POSITION
    }

    position = currentPosition;
    return position;
}

void RotaryEncoder::setMax(int newMaxPosition) {
    maxPosition = newMaxPosition;
}
