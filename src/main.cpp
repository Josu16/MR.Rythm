#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <IntervalTimer.h>

#include "RotaryEncoder.h"
#include "ABRSequencer.h"

// // ----------------------------- SEQUENCER DECLARATION VARIABLES -----------------------------
const int pinA1Re = 2;
const int pinB1Re = 3;
volatile long tempo = 120;
ABRSequencer sequencer(pinA1Re, pinB1Re, tempo);
// // ----------------------------- END SEQUENCER DECLARATION VARIABLES -----------------------------


// ----------------------------- UI DECLARATION VARIABLES -----------------------------
// Pines SPI de hardware en la Teensy 4.1
#define CS 10    // Chip Select para la primera pantalla
#define RS 9     // Reset para la primera pantalla
#define RSE 8    // Registro de datos/comando para la primera pantalla

#define CS_1 35    // Chip Select para la segunda pantalla
#define RS_1 34     // Reset para la segunda pantalla
#define RSE_1 33    // Registro de datos/comando para la segunda pantalla

// Configura las pantallas usando Hardware SPI en lugar de Software SPI
U8G2_ST7565_ERC12864_1_4W_HW_SPI u8g2(U8G2_R0, CS, RS, RSE);
U8G2_ST7565_ERC12864_1_4W_HW_SPI u8g2_1(U8G2_R0, CS_1, RS_1, RSE_1);
// ----------------------------- END UI DECLARATION VARIABLES -----------------------------


// ----------------------------- NAVIGATION DECLARATION VARIABLES -----------------------------

const int pinA2 = 4;
const int pinB2 = 5;
long position2 = 0;
RotaryEncoder encoder2(pinA2, pinB2, position2);

// ----------------------------- END NAVIGATION DECLARATION VARIABLES -----------------------------

void setup() {
    // Inicialización de ambas pantallas
    u8g2.begin();
    u8g2.setContrast(10);
    u8g2.enableUTF8Print();

    u8g2_1.begin();
    u8g2_1.setContrast(10);
    u8g2_1.enableUTF8Print();

    Serial.begin(9600);       // Serial para depuración
    while (!Serial) {
    }
    Serial.println("Iniciando depuración en USB Serial...");

    // Incialización del secuenciador
    sequencer.beginSequencer();
    Serial.println("Secuenciador MIDI Iniciado...");
    
    encoder2.setValue(92);

}

void loop() {
  // Otras tareas, por ejemplo, ajustar el tempo o controlar la interfaz
  position2 = encoder2.update();
  tempo = sequencer.cheeckBpm();

  // Pantalla 1
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_luBS10_tf);
    u8g2.drawFrame(0, 0, 128, 64);
    u8g2.setCursor(6, 25);
    u8g2.print(position2);
    u8g2.drawLine(6, 35, 120, 35);
    u8g2.setCursor(14, 55);
    u8g2.print("BUENOS DÍAS");
  } while (u8g2.nextPage());

  // Pantalla 2
  u8g2_1.firstPage();
  do {
    u8g2_1.setFont(u8g2_font_luBS10_tf);
    u8g2_1.drawFrame(0, 0, 128, 64);
    u8g2_1.setCursor(6, 25);
    u8g2_1.print(tempo);
    u8g2_1.drawLine(6, 35, 120, 35);
    u8g2_1.setCursor(14, 55);
    u8g2_1.print("BUENOS DÍAS");
  } while (u8g2_1.nextPage());

  // delay(3000);

}
