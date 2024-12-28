// pins.h
#ifndef PINS_H
#define PINS_H

// Pines de controles físicos
#define PIN_A1RE 2
#define PIN_B1RE 3
#define PIN_BTN1 41

#define PIN_A2RE 4
#define PIN_B2RE 5
#define PIN_BTN2 40

#define PIN_A3RE 6
#define PIN_B3RE 19
#define PIN_BTN3 39

#define PIN_FW1 22

// Pines de indicadores
#define PIN_LED_TEST 15
#define PIN_LED_PLAYING 14

// pines de pantallas
    //PANTALLA DE REPRODUCCIÓN
#define PIN_CS_PLAY 10 // Chip Select
#define PIN_RS_PLAY 9 // Reset
#define PIN_RSE_PLAY 8 // Registro de datos/comando

    // PANTALA DE PATRÓN
#define PIN_CS_PTRN 35
#define PIN_RS_PTRN 34
#define PIN_RSE_PTRN 33

    // PINES COMPARTIDOS
#define PIN_SCL 13 // SCK (SPI POR DEFECTO NO SE DECLARAN)
#define PIN_ SI 11 // MOSI (SPI POR DEFECTO NO SE DECLARAN)
    

// Pines MIDI (SERIAL 7)
#define PIN_MIDI_TX_7 29

// Pines Salida de Audio 1 (I2S para DAC PCM5102)
#define PIN_I2S_BCLK 21    // Bit Clock
#define PIN_I2S_LRCLK 20   // Left/Right Clock
#define PIN_I2S_DATA 7     // Data In
#define PIN_I2S_MCLK 23    // Master Clock (opcional)

#endif // PINS_H