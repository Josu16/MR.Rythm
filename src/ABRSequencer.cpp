#include "ABRSequencer.h"
#include <Arduino.h>

ABRSequencer* ABRSequencer::instance = nullptr;

// Definición del patrón de eventos
MidiEvent drumPattern[] = {
    // Bombo
    {NOTE_ON, KICK_DRUM, 100, 0},
    {NOTE_OFF, KICK_DRUM, 0, 4},

    {NOTE_ON, KICK_DRUM, 88, 144},
    {NOTE_OFF, KICK_DRUM, 0, 148},

    {NOTE_ON, KICK_DRUM, 109, 192},
    {NOTE_OFF, KICK_DRUM, 0, 196},

    // // Caja
    {NOTE_ON, SNARE_DRUM, 90, 96},
    {NOTE_OFF, SNARE_DRUM, 0, 100},

    {NOTE_ON, SNARE_DRUM, 104, 288},
    {NOTE_OFF, SNARE_DRUM, 0, 304},

    // Hi-Hat
    {NOTE_ON, HI_HAT, 108, 0},
    {NOTE_OFF, HI_HAT, 0, 4},

    {NOTE_ON, HI_HAT, 95, 48},
    {NOTE_OFF, HI_HAT, 0, 52},

    {NOTE_ON, HI_HAT, 111, 96},
    {NOTE_OFF, HI_HAT, 0, 100},

    {NOTE_ON, HI_HAT, 92, 144},
    {NOTE_OFF, HI_HAT, 0, 148},

    {NOTE_ON, HI_HAT, 104, 192},
    {NOTE_OFF, HI_HAT, 0, 200},
    {NOTE_ON, HI_HAT, 91, 240},
    {NOTE_OFF, HI_HAT, 0, 244},
    {NOTE_ON, HI_HAT, 105, 288},
    {NOTE_OFF, HI_HAT, 0, 304},
    {NOTE_ON, HI_HAT, 99, 336},
    {NOTE_OFF, HI_HAT, 0, 340},

    // cencerro
    {NOTE_ON, 56, 108, 0},
    {NOTE_OFF, 56, 0, 4},
    {NOTE_ON, 56, 97, 96},
    {NOTE_OFF, 56, 0, 100},
    {NOTE_ON, 56, 114, 192},
    {NOTE_OFF, 56, 0, 196},
    {NOTE_ON, 56, 110, 288},
    {NOTE_OFF, 56, 0, 302},

    // ride
    {NOTE_ON, 51, 108, 0},
    {NOTE_OFF, 51, 0, 4},
    {NOTE_ON, 51, 97, 96},
    {NOTE_OFF, 51, 0, 100},
    {NOTE_ON, 51, 114, 192},
    {NOTE_OFF, 51, 0, 196},
    {NOTE_ON, 51, 110, 288},
    {NOTE_OFF, 51, 0, 302},
};

ABRSequencer::ABRSequencer(int pinARe, int pinbRe, int pinFw, long bpm)
    : bpmRe(pinARe, pinbRe, bpm)
{
    // secuenciación
    currentTick = 0;
    this -> bpm = bpm;
    lastBpm = this -> bpm;
    isPlaying = false;

    // Controles
    // Rotary Encoder
    bpmRe.setValue(bpm);

    // FootSwitch
    this->pinFw = pinFw;
    pinMode(pinFw, INPUT_PULLUP);
    footswitchChanged = false;
    footswitchState = HIGH;
    lastDebounceTime = 0;
    attachInterrupt(digitalPinToInterrupt(pinFw), fwISR, CHANGE);

    patternLength = sizeof(drumPattern) / sizeof(drumPattern[0]);

    // Inicialización MIDI
    Serial7.begin(31250);     // Serial MIDI a 31,250 baudios

    // Indicadores
    pinMode(14, OUTPUT);
    pinMode(15, OUTPUT);

    // Vincula la instancia actual a la referencia estática
    instance = this;

    // TEMPORAL
    initializePattern();
}

void ABRSequencer::initializePattern() {
    // Limpiar eventList
    for (int i = 0; i < totalTicks; i++) {
        eventList[i].clear();
    }

    // Agregar eventos al eventList
    for (unsigned int i = 0; i < patternLength; i++) {
        MidiEvent event = drumPattern[i];
        int tick = drumPattern[i].tick;

        if (tick >= 0 && tick < totalTicks) {
            eventList[tick].push_back(event);
        } else {
            Serial.println("tick fuera de rango");
        }
    }
}

void ABRSequencer::beginSequencer() {
    updateTimerInterval();
}

void ABRSequencer::updateTimerInterval() {
    uint32_t interval = 60000000 / (bpm * 96); // Intervalo en microsegundos
     // Detén el timer antes de actualizar el intervalo
    mainTimer.end();
    // Reinicia el timer con el nuevo intervalo
    mainTimer.begin(timerCallback, interval);
    // Muestra el intervalo calculado en la consola
    Serial.print("Intervalo actualizado: ");
    Serial.println(interval);
}

void ABRSequencer::timerCallback() {
    if (instance) {
        instance->onTimer();
    }
}

void ABRSequencer::onTimer() {
    // Esta función se ejecuta cada 5208 microsegundos (cuando el tempo está a 120)

    if (!isPlaying){

    }
    else {
        // Control del LED para cada nota negra (96 ticks) y semi-corchea (24 ticks)
        if (currentTick % 96 == 0) {
            // Encender el LED en cada nota negra
            digitalWrite(playLed, HIGH);
            playledState = true;
            playLedOffTick = currentTick + 24; // Apagar el LED después de 24 ticks
        }
        if (playledState && currentTick >= playLedOffTick) {
            // Apagar el LED después de 24 ticks
            digitalWrite(playLed, LOW);
            playledState = false;
        }

        // Procesar todos los eventos en el tick actual
        for (const MidiEvent& event : eventList[currentTick]) {
            // Enviar el evento MIDI
            Serial7.write(event.type);
            Serial7.write(event.note);
            Serial7.write(event.velocity);
        }
        currentTick++;  // Incrementa el contador de ticks

        // Reinicia el patrón después de 384 ticks (4/4 en 96 PPQN)
        if (currentTick >= 384) {
            currentTick = 0;  // Reinicia los ticks
        }
    }
}

// long ABRSequencer::getBpm() {
//     return positionRe;
// }

long ABRSequencer::cheeckBpm() {
    bpm = bpmRe.update();
    // Solo actualiza el timer si el BPM ha cambiado
    if (bpm != lastBpm) {
        updateTimerInterval(); // Ajusta el tempo llamando a la función de actualización del timer
        lastBpm = bpm;         // Actualiza el valor de BPM previo
    }
    return bpm;
}
 void ABRSequencer::fwISR() {
    if (instance) {
        instance->handleFootswitchInterrupt();
    }
}

void ABRSequencer::handleFootswitchInterrupt() {
    footswitchState = digitalRead(pinFw);
    footswitchChanged = true;
}

void ABRSequencer::update() {
    if (footswitchChanged) {
        unsigned long currentTime = millis();
        if ((currentTime - lastDebounceTime) > debounceDelay) {
            if (footswitchState == LOW) {
                isPlaying = !isPlaying;
                
                // UBICACIÓN TEMPORAL
                currentTick = 0;  // Reinicia los ticks
                digitalWrite(playLed, LOW);
            }
            lastDebounceTime = currentTime;
        }
        footswitchChanged = false;
    }
}