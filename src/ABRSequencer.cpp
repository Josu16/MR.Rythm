#include "ABRSequencer.h"
#include <Arduino.h>

ABRSequencer* ABRSequencer::instance = nullptr;

ABRSequencer::ABRSequencer(/*int pinARe, int pinbRe, int pinFw, long bpm, volatile uint32_t *triangleX*/ uint8_t PPQN)
    :
    controls(120, 1), // tempo por defecto
    screens(valuesMainScreen)
{
    // Seteos generales del secuenciador:
    pulsesPerQuarterNote = PPQN;
    
    // Interfaz gráfica:
    valuesMainScreen.numberPtrn = 1;

    // Controles

    // FootSwitch
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

    // this -> triangleX = triangleX;
}

void ABRSequencer::initializePattern() {

    MidiParser parser("001.mid", pattern);
    parser.parseFile();

    patternLength = parser.getNumEvents();

    // for (unsigned int indexEvent = 0; indexEvent < patternLength; indexEvent++) {
    //     Serial.print("Type: ");
    //     Serial.print(pattern.events[indexEvent].type, HEX);
    //     Serial.print(", Note: ");
    //     Serial.print(pattern.events[indexEvent].note);
    //     Serial.print(", Velocity: ");
    //     Serial.print(pattern.events[indexEvent].velocity);
    //     Serial.print(", Tick: ");
    //     Serial.println(pattern.events[indexEvent].tick);
    // }

    // secuenciación 
    currentTick = 0;
    lastBpm = pattern.tempo;
    valuesMainScreen.bpm = lastBpm;
    isPlaying = false;

    // Controles
    controls.setBpm(lastBpm);
    lastPtrn = controls.readPtrn();
    valuesMainScreen.numberPtrn = lastPtrn;
}

void ABRSequencer::beginSequencer() {
    initializePattern();
    updateTimerInterval();
}

void ABRSequencer::updateTimerInterval() {
    uint32_t interval = 60000000 / (valuesMainScreen.bpm * pulsesPerQuarterNote); // Intervalo en microsegundos
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

        for (unsigned int indexEvent = 0; indexEvent < patternLength; indexEvent++) {
            // Enviar el evento MIDI
            // TODO: revisar esta condición, mejorar el cíclo para iterar sobre la misma marcha del avance del tick
            // porque como está actualmente es ineficiente, recorre todos los eventos para buscar los que tocan ser 
            // ejecutados.
            if (pattern.events[indexEvent].tick == currentTick) {
                // Enviar el evento MIDI
                Serial7.write(pattern.events[indexEvent].type);
                Serial7.write(pattern.events[indexEvent].note);
                Serial7.write(pattern.events[indexEvent].velocity);
            }
        }
        updateTrianglePosition();

        currentTick++;  // Incrementa el contador de ticks

        // Reinicia el patrón después de 384 ticks (4/4 en 96 PPQN)
        if (currentTick >= pattern.totalTicks) {
            currentTick = 0;  // Reinicia los ticks
        }
    }
}

void ABRSequencer::updateBpm() {
    valuesMainScreen.bpm = controls.readBpm();
    valuesMainScreen.numberPtrn = controls.readPtrn();
    // Solo actualiza el timer si el BPM ha cambiado
    if (valuesMainScreen.bpm != lastBpm) {
        updateTimerInterval(); // Ajusta el tempo llamando a la función de actualización del timer
        lastBpm = valuesMainScreen.bpm;         // Actualiza el valor de BPM previo
    }
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

void ABRSequencer::loop() {
    // Actualizaciones de pantallas.
    screens.refresh_ui();
    // Verificación de controles físicos.
    updateBpm();
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
    valuesMainScreen.triangleX = map(currentTick, 0, pattern.totalTicks, 10, 117);
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