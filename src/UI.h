#ifndef UI_H
#define UI_H

#include <arduino.h>
#include <U8g2lib.h>
#include <SPI.h>

// Pines SPI de hardware en la Teensy 4.1
#define CS 10    // Chip Select para la primera pantalla
#define RS 9     // Reset para la primera pantalla
#define RSE 8    // Registro de datos/comando para la primera pantalla

#define CS_1 35    // Chip Select para la segunda pantalla
#define RS_1 34     // Reset para la segunda pantalla
#define RSE_1 33    // Registro de datos/comando para la segunda pantalla

struct MainScreen{  // TODO: revisar el nombre final respecto a los nombres de los objetos de pantalla
    uint8_t numberPtrn;
    char namePtrn[15];
    uint8_t measures;
    uint8_t numerator;
    uint8_t denominator;
    volatile uint32_t triangleX = 10;
    volatile uint8_t bpm;
};

class UI {
    private:
        MainScreen &valuesMainScreen;

       // Configura las pantallas usando Hardware SPI en lugar de Software SPI
        U8G2_ST7565_ERC12864_1_4W_HW_SPI u8g2;
        U8G2_ST7565_ERC12864_1_4W_HW_SPI u8g2_1;

        // long patronIndex = 1;
        int ultimopatronIndex = -1;

        // volatile uint32_t triangleX = 10; // Posición actual del triángulo
        const int lineY = 42; // Y de la línea horizontal
        
       // methods
       void draw_dotted_line(U8G2 &u8g2, int x1, int y1, int x2, int y2);

    public:
        UI(MainScreen &values);  // revisar como actualizar cuando entre a otra perspectiva de pantalla (ej. configuración)
        void changeViewScreen();

        void refresh_ui();
};

#endif