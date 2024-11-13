#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
// #include <MIDI.h>

#include "MidiManager.h"

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

// Objeto MIDI manager
MidiManager ABRMidi(Serial7);


void setup(void) {
  // Inicialización de ambas pantallas
  u8g2.begin();
  u8g2.setContrast(10);
  u8g2.enableUTF8Print();

  u8g2_1.begin();
  u8g2_1.setContrast(10);
  u8g2_1.enableUTF8Print();

  // Inicialización del MIDI
  ABRMidi.begin();
  // Serial de consola
  Serial.begin(9600);
  while (!Serial) {

  }
  Serial.println("Iniciando depuración en USB Serial...");
}

void loop(void) {
  delay(1000);           // Espera 500 ms
  ABRMidi.noteOn(9, 36, 100);

  // Serial.println("Enviando Note On:");
  // Serial.print("Status Byte: "); Serial.println(statusByte, HEX);
  // Serial.print("Note: "); Serial.println(note, DEC);
  // Serial.print("Velocity: "); Serial.println(velocity, DEC);
  delay(500);           // Espera 500 ms
  // Pantalla 1
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_luBS10_tf);
    u8g2.drawFrame(0, 0, 128, 64);
    u8g2.setCursor(6, 25);
    u8g2.print("HOA MUNDO ¡");
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
    u8g2_1.print("HOLA MUNDO ¡");
    u8g2_1.drawLine(6, 35, 120, 35);
    u8g2_1.setCursor(14, 55);
    u8g2_1.print("BUENOS DÍAS");
  } while (u8g2_1.nextPage());

  // Serial.println("Enviando Note Off:");
  // Serial.print("Status Byte: "); Serial.println(statusByte, HEX);
  // Serial.print("Note: "); Serial.println(note, DEC);
  // Serial.print("Velocity: "); Serial.println(velocity, DEC);
  ABRMidi.noteOff(9, 36, 100);
  delay(1000); // Espera de 1 segundo
}