#include "ABRSequencer.h"
#include <Arduino.h>

ABRSequencer* ABRSequencer::instance = nullptr;

ABRSequencer::ABRSequencer(int pinARe, int pinbRe, int pinFw, long bpm, volatile uint32_t *triangleX)
    : bpmRe(pinARe, pinbRe, bpm)
{
    // secuenciación  TEMPORAL!!!!!!!!!!!!!!!!
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


    // Inicialización MIDI
    Serial7.begin(31250);     // Serial MIDI a 31,250 baudios

    // Indicadores
    pinMode(14, OUTPUT);
    pinMode(15, OUTPUT);

    // Vincula la instancia actual a la referencia estática
    instance = this;

    this -> triangleX = triangleX;
}

void ABRSequencer::initializePattern() {

    MidiParser parser("001.mid", eventList);
    parser.parseFile();

    patternLength = parser.getNumEvents();

    for (const auto& e : eventList) {
        Serial.print("Type: ");
        Serial.print(static_cast<int>(e.type), HEX);
        Serial.print(", Note: ");
        Serial.print(static_cast<int>(e.note));
        Serial.print(", Velocity: ");
        Serial.print(static_cast<int>(e.velocity));
        Serial.print(", Tick: ");
        Serial.println(e.tick);
    }

}

void ABRSequencer::beginSequencer() {
    initializePattern();
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
        for (const auto& event : eventList) {
            // Enviar el evento MIDI
            if (event.tick == currentTick) {
                // Enviar el evento MIDI
                Serial7.write(event.type);
                Serial7.write(event.note);
                Serial7.write(event.velocity);
            }
        }
        updateTrianglePosition();

        currentTick++;  // Incrementa el contador de ticks

        // Reinicia el patrón después de 384 ticks (4/4 en 96 PPQN)
        if (currentTick >= 768) {
            currentTick = 0;  // Reinicia los ticks
        }
    }
}

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
                
                // TODO: APAGAR TODAS LAS NOTAS QUE HAYAN PERMANECIDO ACTIVAS
                allNotesOff(1);
            }
            lastDebounceTime = currentTime;
        }
        footswitchChanged = false;
    }
}

uint32_t ABRSequencer::getCurrentTick() {
    return currentTick;
}

void ABRSequencer::updateTrianglePosition() {
  if (currentTick % 24 == 0) { // cambia la posición del triángulo cada Semicorchea.
    // Actualiza la posición del triángulo
    *triangleX = map(currentTick, 0, 768, 10, 117);
  }
}

void ABRSequencer::allNotesOff(uint8_t channel) {
    // Asegúrate de que el canal esté en el rango MIDI (1-16)
    if (channel < 1 || channel > 16) return;

    uint8_t status = 0xB0 | (channel - 1); // Status byte: Control Change en el canal
    uint8_t controller = 123;             // Número de controlador: All Notes Off
    uint8_t value = 0;                    // 3er byte: Siempre 0x00

    // Enviar el mensaje MIDI
    Serial7.write(status);   // Enviar el Status byte
    Serial7.write(controller); // Enviar el número de controlador
    Serial7.write(value);    // Enviar el valor (0)
}