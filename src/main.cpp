#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <IntervalTimer.h>

#include "RotaryEncoder.h"
#include "ABRSequencer.h"

// // ----------------------------- SEQUENCER DECLARATION VARIABLES -----------------------------
ABRSequencer sequencer(/*pinA1Re, pinB1Re, pinFw, 120, &triangleX*/ 96);
// // ----------------------------- END SEQUENCER DECLARATION VARIABLES -----------------------------

void setup() {

    Serial.begin(9600);       // Serial para depuración
    while (!Serial) {
    }
    Serial.println("Iniciando depuración en USB Serial...");

    // Incialización del secuenciador
    sequencer.beginSequencer();
    Serial.println("Secuenciador MIDI Iniciado...");
}

void loop() {
  sequencer.loop();
}