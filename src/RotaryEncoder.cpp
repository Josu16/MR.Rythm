// RotaryEncoder.cpp
#include "RotaryEncoder.h"
#include <Arduino.h>

RotaryEncoder::RotaryEncoder(int pinA, int pinB, long &position, unsigned long debounceDelay)
    : encoder(pinA, pinB), position(position), debounceDelay(debounceDelay) {
    lastPosition = encoder.read() / 4;
    lastDebounceTime = 0;
}

void RotaryEncoder::update() {
    long currentPosition = encoder.read() / 4;
    if (currentPosition != lastPosition && (millis() - lastDebounceTime) > debounceDelay) {
        position = currentPosition;
        lastPosition = currentPosition;
        lastDebounceTime = millis();
    }
}
