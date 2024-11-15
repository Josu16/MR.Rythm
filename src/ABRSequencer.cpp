#include "ABRSequencer.h"
#include <Arduino.h>

ABRSequencer* ABRSequencer::instance = nullptr;

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

    /// Inicialización del drumPattern
    drumPattern[0] = {NOTE_ON, KICK_DRUM, 100, 0};    // Bombo en el primer tick
    drumPattern[1] = {NOTE_OFF, KICK_DRUM, 100, 8};   // Apagar bombo en el tick 48
    drumPattern[2] = {NOTE_ON, KICK_DRUM, 83, 192};   // Caja en el segundo cuarto de nota
    drumPattern[3] = {NOTE_OFF, KICK_DRUM, 83, 200};  // Apagar caja en el tick 144

    patternLength = sizeof(drumPattern) / sizeof(drumPattern[0]);
    eventIndex = 0;  // Índice del evento actual

    // Inicialización MIDI
    Serial7.begin(31250);     // Serial MIDI a 31,250 baudios

    // Indicadores
    pinMode(14, OUTPUT);
    pinMode(15, OUTPUT);

    // Vincula la instancia actual a la referencia estática
    instance = this;

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

        // Verifica si el evento actual debe reproducirse en el tick actual
        if (eventIndex < patternLength && this->drumPattern[eventIndex].tick == currentTick) {
            MidiEvent event = drumPattern[eventIndex];
            
            // Enviar el evento MIDI en Serial7 (puerto MIDI)
            Serial7.write(event.type);
            Serial7.write(event.note);
            Serial7.write(event.velocity);

            // Avanza al siguiente evento
            eventIndex++;
        }
        currentTick++;  // Incrementa el contador de ticks

        // Reinicia el patrón después de 384 ticks (4/4 en 96 PPQN)
        if (currentTick >= 384) {
            currentTick = 0;  // Reinicia los ticks
            eventIndex = 0;   // Reinicia el índice de eventos
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
            }
            lastDebounceTime = currentTime;
        }
        footswitchChanged = false;
    }
}