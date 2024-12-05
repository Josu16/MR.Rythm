#include "UI.h"
#include <Arduino.h>

const char* patrones[] = {
  "Margarita 1a c",
  "Oye como va",
  "Bolero 2",
  "Bolero 3",
  "Bolero ritmico",
  "Bossa nova",
  "Big Samba",
  "Europa 1",
  "Vals Arillo",
  "Salsa Universal",
  "Puente de piedr",
  "Cumbia Carro sh",
  "Cumbia texana",
  "Cumbia sonora",
  "Mambo",
  "Merengue",
  "Chilena Tomasin",
  "FIN DE LISTA",
  "FIN DE LISTA",
  "FIN DE LISTA",
  "FIN DE LISTA",
  "FIN DE LISTA",
  "FIN DE LISTA",
  "FIN DE LISTA",
  "FIN DE LISTA",
  "FIN DE LISTA",
  "FIN DE LISTA",
  "FIN DE LISTA",
  "FIN DE LISTA",
  "FIN DE LISTA",
  "FIN DE LISTA",
  "FIN DE LISTA",
  "FIN DE LISTA",
  "FIN DE LISTA",
  "FIN DE LISTA",
  "FIN DE LISTA",
  "FIN DE LISTA",
  "FIN DE LISTA",
  "FIN DE LISTA",
  "FIN DE LISTA",
};

UI::UI(MainScreen &values)
    :
    valuesMainScreen(values),
    u8g2(U8G2_R0, CS, RS, RSE),
    u8g2_1(U8G2_R0, CS_1, RS_1, RSE_1)
  {
    // Inicialización de ambas pantallas
    u8g2.begin();
    u8g2.setContrast(10);
    u8g2.enableUTF8Print();

    u8g2_1.begin();
    u8g2_1.setContrast(15);
    u8g2_1.enableUTF8Print();

    // patronIndex = 1;
}

void UI::draw_dotted_line(U8G2 &u8g2, int x1, int y1, int x2, int y2) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    int steps = max(abs(dx), abs(dy));
    float x_inc = dx / (float)steps;
    float y_inc = dy / (float)steps;

    float x = x1;
    float y = y1;

    for (int i = 0; i <= steps; i++) {
        if ((i % 3) != 2) { // Pinta en los pasos 0, 1, 3, 4, 6, 7, etc. (dos encendidos, uno apagado)
            u8g2.drawPixel((int)x, (int)y);
        }
        x += x_inc;
        y += y_inc;
    }
}

void UI::refresh_ui() {
    // Pantalla 1
    u8g2.firstPage();
    do {
        u8g2.setFont(u8g2_font_luBS10_tf);
        u8g2.drawFrame(0, 0, 128, 64);
        u8g2.setCursor(6, 25);
        u8g2.print(valuesMainScreen.numberPtrn);
        u8g2.drawLine(6, 35, 120, 35);
        u8g2.setCursor(14, 55);
        u8g2.print("BUENOS DÍAS");
    } while (u8g2.nextPage());

    // Pantalla 2
    // Detecta si patronIndex cambió
    if (valuesMainScreen.numberPtrn != ultimopatronIndex) {
        ultimopatronIndex = valuesMainScreen.numberPtrn;
    }

    u8g2_1.firstPage();
    do {
        u8g2_1.setFont(u8g2_font_crox4t_tf); // opción má acertada


        // NOMBRE DEL PATRÓN (centrado o deslizante)
        u8g2_1.setCursor(0, 35); // Cambia "55" si necesitas ajustar la posición vertical
        u8g2_1.print(patrones[valuesMainScreen.numberPtrn]);

        // número del patrón
        u8g2_1.setFont(u8g2_font_luBS18_tf);
        u8g2_1.setCursor(-1, 18);
        char buffer[4]; // 3 dígitos + terminador nulo
        // Formateamos el número con ceros a la izquierda
        snprintf(buffer, sizeof(buffer), "%03d", valuesMainScreen.numberPtrn);
        u8g2_1.print(buffer);

        // Indicador de compás rítmico
        u8g2_1.drawBox(53, 0, 74, 9); // Dibuja un rectángulo negro donde estará el text
        u8g2_1.setDrawColor(0); // Color de dibujo blanc
        // draw_dotted_line(u8g2_1, 54, 9, 127, 9);
        // u8g2_1.setFont(u8g2_font_squeezed_b7_tr); // ES UNA buena fuente pero no me convence mucho aún
        u8g2_1.setFont(u8g2_font_squeezed_b7_tr);
        u8g2_1.setCursor(110, 8);
        u8g2_1.print("6/8");

        // Indicador de tipo de secuencia
        u8g2_1.setFont(u8g2_font_5x7_mr);
        // u8g2_1.setFont(u8g2_font_lucasfont_alternate_tf);
        u8g2_1.setCursor(54, 8);
        u8g2_1.print("PATRON");

        // indicador de número de compases
        u8g2_1.setFont(u8g2_font_timR08_tn);
        u8g2_1.setCursor(94, 8);
        u8g2_1.print("4");
        u8g2_1.setDrawColor(1); // Color de dibujo negro

        u8g2_1.setFont(u8g2_font_04b_03b_tr);
        u8g2_1.setCursor(73, 17);
        u8g2_1.print("001 - 1");

        // u8g2_1.drawBox(53, 0, 74, 9); // Dibuja un rectángulo negro donde estará el text
        // u8g2_1.setCursor(94, 17);
        // u8g2_1.print("3");

        // linea divisora
        u8g2_1.drawTriangle(
        valuesMainScreen.triangleX, lineY,    // Vértice superior
        valuesMainScreen.triangleX - 3, lineY - 3,    // Esquina inferior izquierda
        valuesMainScreen.triangleX + 3, lineY - 3     // Esquina inferior derecha
        );
        u8g2_1.drawLine(10, 43, 117, 43);
        u8g2_1.drawPixel(10, 44);
        u8g2_1.drawPixel(117, 44);
        u8g2_1.drawLine(10, 45, 117, 45);
        // u8g2_1.drawBox(0, 44, 15, 20); // Dibuja un recuadro (x, y, ancho, alto)
        // u8g2_1.setFont(u8g2_font_6x13B_tf); // FUENTE MUY JUNTA

        // u8g2_1.drawBox(47, 46, 28, 18); // Dibuja un rectángulo negro donde estará el text

        u8g2_1.setFont(u8g2_font_tiny5_tf);
        u8g2_1.setCursor(48, 64);
        u8g2_1.print("TIEMPO");
        
        u8g2_1.setFont(u8g2_font_prospero_bold_nbp_tn);
        // u8g2_1.setFont(u8g2_font_Born2bSportyV2_tr);
        u8g2_1.setCursor(51, 57);
        // u8g2_1.setFont(u8g2_font_t0_17b_tf); // posible fuente para quedarse
        // u8g2_1.setFont(u8g2_font_profont17_tf); // FINAL
        // u8g2_1.setFont(u8g2_font_koleeko_tn); //  fuente muy buena para números
        u8g2_1.print(valuesMainScreen.bpm);
        // u8g2_1.drawLine(4, 64, 20, 127)

        // u8g2_1.setFont(u8g2_font_Born2bSportyV2_tr);
        // u8g2_1.setCursor(4, 60);
        // u8g2_1.print("01 - 1");


    } while (u8g2_1.nextPage());
}