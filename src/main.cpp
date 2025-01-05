#include <Arduino.h>
#include "RotaryEncoder.h"
#include "ABRSequencer.h"

// // ----------------------------- SEQUENCER DECLARATION VARIABLES -----------------------------
ABRSequencer sequencer(96);
// // ----------------------------- END SEQUENCER DECLARATION VARIABLES -----------------------------

void setup() {
    // Incialización del secuenciador
    sequencer.beginSequencer();
    Serial.println("Secuenciador MIDI Iniciado...");
}

void loop() {
  sequencer.loop();
}