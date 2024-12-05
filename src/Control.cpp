#include "Control.h"

Control* Control::instance = nullptr;

Control::Control(volatile long tempo, volatile long pattern)
    :
    bpmRE(pinA1Re, pinB1Re, tempo, 20, 300),
    ptrnRE(pinA2Re, pinB2Re, pattern, 1, 10) // TODO: revisar el parámetro long. corrgir.
{
    // FootSwitch
    pinMode(pinFw, INPUT_PULLUP);
    footswitchChanged = false;
    footswitchState = HIGH;
    lastDebounceTime = 0;
    attachInterrupt(digitalPinToInterrupt(pinFw), fwISR, CHANGE);

    instance = this;
}

long Control::readBpm() {
    return bpmRE.getPosition();
}

void Control::setBpm(int newValue) {
    bpmRE.setPosition(newValue);
}

long Control::readPtrn() {
    return ptrnRE.getPosition();
}

void Control::setPtrn(int newValue) {
    ptrnRE.setPosition(newValue);
}

void Control::fwISR() {
    if (instance) {
        instance->handleFootswitchInterrupt();
    }
}
void Control::handleFootswitchInterrupt() {
    footswitchState = digitalRead(pinFw);
    footswitchChanged = true;
}

bool Control::checkForFootswitch() {
    if (footswitchChanged) {
        unsigned long currentTime = millis();
        if ((currentTime - lastDebounceTime) > debounceDelay) {
            lastDebounceTime = currentTime; // Actualizar el tiempo de debounce
            footswitchChanged = false;     // Reiniciar la marca de cambio
            return (footswitchState == LOW); // Devuelve si el footswitch está activo
        }
    }
    return false; // No hay cambio válido
}