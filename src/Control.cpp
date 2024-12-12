#include "Control.h"

Control* Control::instance = nullptr;

Control::Control(volatile long tempo, volatile long pattern)
    :
    bpmRE(pinA1Re, pinB1Re, tempo, 20, 250),
    ptrnRE(pinA2Re, pinB2Re, pattern, 1, 10), // TODO: revisar el parámetro long. corrgir.
    variantRE(pinA3Re, pinB3Re, 1, 1, 5) // TODO: revisar el parámetro long. corrgir.
{
    // FootSwitch
    pinMode(pinFw, INPUT_PULLUP);
    footswitchChanged = false;
    footswitchState = HIGH;
    lastDebounceTime = 0;
    attachInterrupt(digitalPinToInterrupt(pinFw), fwISR, CHANGE);

    // LoockTempo
    pinMode(pinLoockTempo, INPUT_PULLUP);
    loockTempoState = false;
    loockTempoChanged = false;
    attachInterrupt(digitalPinToInterrupt(pinLoockTempo), loockTempoISR, RISING);

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

void Control::setVariant(int newValue) {
    variantRE.setPosition(newValue);
}

void Control::setMaxVariant(int newValue) {
    variantRE.setMax(newValue);
}

long Control::readVariant() {
    return variantRE.getPosition();
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

void Control::loockTempoISR() {
    if (instance) {
        instance->handleLoockTempooInterrupt();
    }
}

// REVISAR ESTA FUNCIÓN
/*
Cuando se recibe una interrupción, esta función entra muchas veces por presión del botón, es importante
tener un mecanismo de tratamiento de interrumpir tantas veces, porque esto puede entrar en conflicto 
con la interrupción más importante que es el timer.
*/
void Control::handleLoockTempooInterrupt() {
        loockTempoTriggered = true;
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

bool Control::cheeckForLoockTempo() {
    if (loockTempoTriggered) {
        unsigned long currentTime = millis();
        if ((currentTime - lastDebounceTimeTempo) > debounceDelayTempo) {
            lastDebounceTime = currentTime;
            loockTempoState = !loockTempoState;
        }
        loockTempoTriggered = false;
    }
    return loockTempoState;
}