#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <IntervalTimer.h>

#include "RotaryEncoder.h"
#include "ABRSequencer.h"
// #include "MidiParser.h"

// // ----------------------------- SEQUENCER DECLARATION VARIABLES -----------------------------
ABRSequencer sequencer(/*pinA1Re, pinB1Re, pinFw, 120, &triangleX*/ 96);
// // ----------------------------- END SEQUENCER DECLARATION VARIABLES -----------------------------

// ----------------------------- NAVIGATION DECLARATION VARIABLES -----------------------------

const int pinA2 = 4;
const int pinB2 = 5;

// RotaryEncoder ptrnEncoder(pinA2, pinB2, patronIndex);

// ----------------------------- END NAVIGATION DECLARATION VARIABLES -----------------------------
// zona temporal
// const char* textoDeslizante = "Cumbia Sonora";

// const int totalTextos = sizeof(patrones) / sizeof(patrones[0]); // Número total de textos

volatile uint32_t currentStep = 0; // Posición actual del paso (96 PPQN)
const int seqLength = 384; // Longitud de la secuencia (en pasos)
const int screenWidth = 117;


unsigned long lastUpdateTime = 0; // Última vez que se actualizó el triángulo
const unsigned long updateInterval = 190; // Intervalo en ms para actualizar


void setup() {

    Serial.begin(9600);       // Serial para depuración
    while (!Serial) {
    }
    Serial.println("Iniciando depuración en USB Serial...");

    // Incialización del secuenciador
    sequencer.beginSequencer();
    Serial.println("Secuenciador MIDI Iniciado...");

    // ptrnEncoder.setValue(1);

}

void loop() {
  // Otras tareas, por ejemplo, ajustar el tempo o controlar la interfaz
  // patronIndex = ptrnEncoder.update();
  // tempo = sequencer.cheeckBpm();
  sequencer.loop();

}