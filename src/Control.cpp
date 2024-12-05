#include "Control.h"

Control::Control(volatile long tempo, volatile long pattern)
    :
    bpmRE(pinA1Re, pinB1Re, tempo, 20, 300),
    ptrnRE(pinA2Re, pinB2Re, pattern, 1, 10) // TODO: revisar el parámetro long. corrgir.
{
        Serial.println("a");
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