// RotaryEncoder.cpp
#include "RotaryEncoder.h"
#include <Arduino.h>

RotaryEncoder::RotaryEncoder(int pinA, int pinB, long &position, unsigned long debounceDelay)
    : encoder(pinA, pinB), position(position), debounceDelay(debounceDelay) {
    lastPosition = encoder.read() / 4;
    lastDebounceTime = 0;
}

void RotaryEncoder::setValue(int newValue) {
    encoder.write(newValue*4);
}

// long RotaryEncoder::getValue() {
//     return encoder.read();
// }

long RotaryEncoder::update() {
    long currentPosition = encoder.read() / 4;
    // if (currentPosition < 20) {
    //         currentPosition = 20;
    //         encoder.write(20);
    //     }
    //     else if (currentPosition > 260) {
    //         currentPosition = 260;
    //         encoder.write(260);
    //     }
    // if (currentPosition != lastPosition && (millis() - lastDebounceTime) > debounceDelay) {
    //     position = currentPosition;
    //     lastPosition = currentPosition;
    //     lastDebounceTime = millis();
        
    // }
    return currentPosition;
}
