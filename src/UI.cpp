#include "UI.h"
#include <Arduino.h>

UI::UI(MainScreen &values)
    :
    valuesMainScreen(values),
    playScreen(U8G2_R0, CS, RS, RSE),
    ptrScreen(U8G2_R0, CS_1, RS_1, RSE_1)
  {
    // Inicialización de ambas pantallas
    playScreen.begin();
    playScreen.setContrast(10);
    playScreen.enableUTF8Print();

    ptrScreen.begin();
    ptrScreen.setContrast(15);
    ptrScreen.enableUTF8Print();

    // patronIndex = 1;
}

void UI::draw_dotted_line(U8G2 &playScreen, int x1, int y1, int x2, int y2) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    int steps = max(abs(dx), abs(dy));
    float x_inc = dx / (float)steps;
    float y_inc = dy / (float)steps;

    float x = x1;
    float y = y1;

    for (int i = 0; i <= steps; i++) {
        if ((i % 3) != 2) { // Pinta en los pasos 0, 1, 3, 4, 6, 7, etc. (dos encendidos, uno apagado)
            playScreen.drawPixel((int)x, (int)y);
        }
        x += x_inc;
        y += y_inc;
    }
}

void UI::refreshUi() {
    refreshPlayScreen();
    refreshPtrnScreen();
}

void UI::refreshPlayScreen() {
    // Pantalla 1
    playScreen.firstPage();
    do {
        playScreen.setFont(u8g2_font_luBS10_tf);
        playScreen.drawFrame(0, 0, 128, 64);
        playScreen.setCursor(6, 25);
        playScreen.print(valuesMainScreen.numberPtrn);
        playScreen.drawLine(6, 35, 120, 35);
        playScreen.setCursor(14, 55);
        playScreen.print("BUENOS DÍAS");
    } while (playScreen.nextPage());

    // Pantalla 2
    // Detecta si patronIndex cambió
    if (valuesMainScreen.numberPtrn != ultimopatronIndex) {
        ultimopatronIndex = valuesMainScreen.numberPtrn;
    }
}

void UI::refreshPtrnScreen() {
    ptrScreen.firstPage();
    do {
        int16_t textWidth, x;

        if (valuesMainScreen.waitingForChangePtrn == true) {
            ptrScreen.drawBox(0, 20, 127, 18); // Dibuja un rectángulo negro donde estará el text
            ptrScreen.setDrawColor(0); // Color de dibujo blanco
            //  ----------- NOMBRE DEL PATRÓN (centrado)
            ptrScreen.setFont(u8g2_font_t0_17b_me); // perfecta para 14 cara
            textWidth = ptrScreen.getUTF8Width(valuesMainScreen.namePtrn);
            // Calcula la posición horizontal para centrar
            x = (128 - textWidth) / 2; // 128 es el ancho de la pantalla
            ptrScreen.setCursor(x, 34); // Cambia "55" si necesitas ajustar la posición vertical
            ptrScreen.print(valuesMainScreen.namePtrn);
            ptrScreen.setDrawColor(1); // Color de dibujo negro
        } else {
            //  ----------- NOMBRE DEL PATRÓN (centrado)
            ptrScreen.setFont(u8g2_font_t0_17b_me); // perfecta para 14 cara
            textWidth = ptrScreen.getUTF8Width(valuesMainScreen.namePtrn);
            // Calcula la posición horizontal para centrar
            x = (128 - textWidth) / 2; // 128 es el ancho de la pantalla
            ptrScreen.setCursor(x, 34); // Cambia "55" si necesitas ajustar la posición vertical
            ptrScreen.print(valuesMainScreen.namePtrn);
        }

        // ----------- número del patrón
        ptrScreen.setFont(u8g2_font_luBS18_tf);
        ptrScreen.setCursor(-1, 18);
        char buffer[4]; // 3 dígitos + terminador nulo
        // Formateamos el número con ceros a la izquierda
        snprintf(buffer, sizeof(buffer), "%03d", valuesMainScreen.numberPtrn);
        ptrScreen.print(buffer);

        // ----------- Inversión de colores
        ptrScreen.drawBox(53, 0, 74, 9); // Dibuja un rectángulo negro donde estará el text
        ptrScreen.setDrawColor(0); // Color de dibujo blanco

        // ----------- Indicador de compás rítmico
        ptrScreen.setFont(u8g2_font_squeezed_b7_tr);
        ptrScreen.setCursor(110, 8);
        ptrScreen.print(valuesMainScreen.numerator);
        ptrScreen.print("/");
        ptrScreen.print(valuesMainScreen.denominator);

        // ----------- Indicador de tipo de secuencia
        ptrScreen.setFont(u8g2_font_5x7_mr);
        // ptrScreen.setFont(u8g2_font_lucasfont_alternate_tf);
        ptrScreen.setCursor(54, 8);
        ptrScreen.print("PATRON");

        // ----------- Indicador de número de compases
        ptrScreen.setFont(u8g2_font_timR08_tn);
        ptrScreen.setCursor(94, 8);
        ptrScreen.print(valuesMainScreen.measures);

        // ----------- Inversión de colores
        ptrScreen.setDrawColor(1); // Color de dibujo negro

        // ----------- Conteo de compases
        ptrScreen.setFont(u8g2_font_04b_03b_tr);
        ptrScreen.setCursor(73, 17);
        snprintf(buffer, sizeof(buffer), "%03d", valuesMainScreen.currentMeasure);
        ptrScreen.print(buffer);
        ptrScreen.print(" - ");
        ptrScreen.print(valuesMainScreen.currentBlack);

        // ----------- Triángulo de secuencia
        ptrScreen.drawTriangle(
        valuesMainScreen.triangleX, lineY,    // Vértice superior
        valuesMainScreen.triangleX - 2, lineY - 3,    // Esquina inferior izquierda
        valuesMainScreen.triangleX + 3, lineY - 3     // Esquina inferior derecha
        );
        // ----------- Linea divisora
        ptrScreen.drawLine(10, 43, 117, 43);
        ptrScreen.drawPixel(10, 44);
        ptrScreen.drawPixel(117, 44);
        ptrScreen.drawLine(10, 45, 117, 45);

        // ----------- Tempo
        ptrScreen.setFont(u8g2_font_tiny5_tf);
        ptrScreen.setCursor(51, 64);
        ptrScreen.print("TIEMPO");
        
        ptrScreen.setFont(u8g2_font_prospero_bold_nbp_tn);

        // Convierte el valor uint8_t a texto
        sprintf(buffer, "%u", valuesMainScreen.bpm); // Convierte el número a string
        // Calcula el ancho del texto
        textWidth = ptrScreen.getUTF8Width(buffer);
        // Calcula la posición horizontal para centrar
        x = (128 - textWidth) / 2; // 128 es el ancho de la pantalla
        // Configura el cursor en la posición calculada
        ptrScreen.setCursor(x, 57);

        if (valuesMainScreen.loockTempo) {
            ptrScreen.drawBox(51, 47, 26, 11); // Dibuja un rectángulo negro donde estará el text
            ptrScreen.setDrawColor(0); // Color de dibujo blanco
            ptrScreen.print(buffer);
            ptrScreen.setDrawColor(1); // Color de dibujo blanco
        }
        else {
            ptrScreen.print(buffer);
        }
        // Dibuja el texto en la pantalla

        // ----------- Variación
        paintVariation();

    } while (ptrScreen.nextPage());
}

void UI::paintVariation() {
    // ----------- Variaciones directas de secuencia.
    ptrScreen.setCursor(6, 52);
    ptrScreen.setFont(u8g2_font_threepix_tr);
    ptrScreen.print("VARIACION");

    ptrScreen.setFont(u8g2_font_tom_thumb_4x6_mf);

    const uint8_t distanceVariants = 8;
    uint8_t pxCurrentVariant = 7;
    // const uint8_t distanceVariants = 8;
    for (uint8_t variation = 1; variation <= 5; variation++) {
        if (valuesMainScreen.currentVariationIndex == variation) {
            ptrScreen.drawBox(pxCurrentVariant - 2, 54, 7, 7); // Dibuja un rectángulo negro donde estará el text
            ptrScreen.setDrawColor(0); // Color de dibujo blanco
            ptrScreen.setCursor(pxCurrentVariant, 60);
            ptrScreen.print(variation);
            pxCurrentVariant += distanceVariants;
            ptrScreen.setDrawColor(1); // Color de dibujo negro
        }
        else {
            ptrScreen.setCursor(pxCurrentVariant, 60);
            ptrScreen.print(variation);
            pxCurrentVariant += distanceVariants;
        }
    }
}