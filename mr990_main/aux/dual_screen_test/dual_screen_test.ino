#include <U8g2lib.h>
#include <SPI.h>

#define SCL 13   // Reloj
#define SI 11    // Datos (MOSI)
#define CS 10    // Chip Select
#define RS 9     // Reset
#define RSE 8    // Registro de datos/comando

#define SCL_1 37   // Reloj
#define SI_1 36    // Datos (MOSI)
#define CS_1 35    // Chip Select
#define RS_1 34     // Reset
#define RSE_1 33    // Registro de datos/comando

U8G2_ST7565_ERC12864_1_4W_SW_SPI u8g2(U8G2_R0, SCL, SI, CS, RS, RSE);

U8G2_ST7565_ERC12864_1_4W_SW_SPI u8g2_1(U8G2_R0, SCL_1, SI_1, CS_1, RS_1, RSE_1);


void setup(void) {
  u8g2.begin(); // Inicializa
  u8g2.setContrast (10); //contraste
  u8g2.enableUTF8Print(); //Visu on

  u8g2_1.begin(); // Inicializa
  u8g2_1.setContrast (10); //contraste
  u8g2_1.enableUTF8Print(); //Visu on
}

void loop(void) {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_luBS10_tf); // Fuente Lucida 10px
    u8g2.drawFrame(0, 0, 128, 64);     // Dibuja un marco de 128x64
    u8g2.setCursor(6, 25);
    u8g2.print("HOLA MUNDO ¡");        // Muestra el mensaje
    u8g2.drawLine(6, 35, 120, 35);     // Línea horizontal
    u8g2.setCursor(14, 55);
    u8g2.print("BUENOS DÍAS");         // Segundo mensaje
  } while (u8g2.nextPage());

  u8g2_1.firstPage();
  do {
    u8g2_1.setFont(u8g2_font_luBS10_tf); // Fuente Lucida 10px
    u8g2_1.drawFrame(0, 0, 128, 64);     // Dibuja un marco de 128x64
    u8g2_1.setCursor(6, 25);
    u8g2_1.print("HOLA MUNDO ¡");        // Muestra el mensaje
    u8g2_1.drawLine(6, 35, 120, 35);     // Línea horizontal
    u8g2_1.setCursor(14, 55);
    u8g2_1.print("BUENOS DÍAS");         // Segundo mensaje
  } while (u8g2_1.nextPage());
  
  delay(1000); // Espera de 1 segundo
}
