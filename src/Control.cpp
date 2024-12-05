#include "Control.h"

Control::Control(volatile long tempo, volatile long pattern)
    :
    bpmRE(pinA1Re, pinB1Re, tempo),
    ptrnRE(pinA2Re, pinB2Re, pattern) // TODO: revisar el parámetro long. corrgir.
{
        Serial.println("a");
}

long Control::readBpm() {
    return bpmRE.update();
}

void Control::setBpm(int newValue) {
    bpmRE.setValue(newValue);
}

long Control::readPtrn() {
    return ptrnRE.update();
}

void Control::setPtrn(int newValue) {
    ptrnRE.setValue(newValue);
}