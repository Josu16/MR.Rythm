#include <Arduino.h>
#include "ABRSequencer.h"

// // ----------------------------- SEQUENCER DECLARATION VARIABLES -----------------------------
ABRSequencer sequencer(96);
// // ----------------------------- END SEQUENCER DECLARATION VARIABLES -----------------------------

void setup() {
    // Incialización del secuenciador
    sequencer.beginSequencer();
    Serial.println("Secuenciador MIDI Iniciado...");
    Serial.print("Velocidad del cpu");
    Serial.println(F_CPU);
}

void loop() {
  sequencer.loop();
}