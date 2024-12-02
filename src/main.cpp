#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <IntervalTimer.h>

#include "RotaryEncoder.h"
#include "ABRSequencer.h"
// #include "MidiParser.h"

// // ----------------------------- SEQUENCER DECLARATION VARIABLES -----------------------------
const int pinA1Re = 2;
const int pinB1Re = 3;
const int pinFw = 41;
volatile long tempo = 120;

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
long patronIndex = 1;
RotaryEncoder ptrnEncoder(pinA2, pinB2, patronIndex);

// ----------------------------- END NAVIGATION DECLARATION VARIABLES -----------------------------
// zona temporal
// const char* textoDeslizante = "Cumbia Sonora";
const char* patrones[] = {
  "Margarita 1a cond",
  "Oye como va",
  "Bolero 2",
  "Bolero 3",
  "Bolero ritmico",
  "Bossa nova",
  "Big Samba",
  "Europa 1",
  "Vals Arillo",
  "Salsa Universal",
  "Puente de piedra",
  "Cumbia Carro show",
  "Cumbia texana",
  "Cumbia sonora",
  "Mambo",
  "Merengue",
  "Chilena Tomasines",
  "FIN DE LA LISTA",
  "FIN DE LA LISTA",
  "FIN DE LA LISTA",
  "FIN DE LA LISTA",
  "FIN DE LA LISTA",
  "FIN DE LA LISTA",
  "FIN DE LA LISTA",
  "FIN DE LA LISTA",
  "FIN DE LA LISTA",
  "FIN DE LA LISTA",
  "FIN DE LA LISTA",
  "FIN DE LA LISTA",
  "FIN DE LA LISTA",
  "FIN DE LA LISTA",
  "FIN DE LA LISTA",
  "FIN DE LA LISTA",
  "FIN DE LA LISTA",
  "FIN DE LA LISTA",
  "FIN DE LA LISTA",
  "FIN DE LA LISTA",
  "FIN DE LA LISTA",
  "FIN DE LA LISTA",
  "FIN DE LA LISTA",
};
const int totalTextos = sizeof(patrones) / sizeof(patrones[0]); // Número total de textos

int anchoTexto;           // Ancho del texto en píxeles
int anchoPantalla = 128;  // Ancho de la pantalla
int xOffset;              // Posición actual del texto

// Variables para el control del tiempo
unsigned long tiempoAnterior = 0;
unsigned long intervaloEsperaInicial = 1500; // Tiempo en milisegundos para esperar en estado estático
unsigned long intervaloEsperaFinal = 800;  // Tiempo en milisegundos para esperar al final
unsigned long intervaloDesplazamiento = 120; // Velocidad del desplazamiento

// Estados de la máquina
enum Estado { ESPERA_INICIAL, MOVIENDO, ESPERA_FINAL, REINICIO };
Estado estadoActual = ESPERA_INICIAL;

void draw_dotted_line(U8G2 &u8g2, int x1, int y1, int x2, int y2) {
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

void actualizarTexto() {
  if (patronIndex < 1 || patronIndex >= totalTextos) {
    patronIndex = 1; // Asegura que el índice esté dentro del rango válido
  }
  u8g2_1.setFont(u8g2_font_crox4t_tf); // Establece la fuente
  anchoTexto = u8g2_1.getStrWidth(patrones[patronIndex]); // Calcula el ancho del texto actual
  // xOffset = 0;
}
int ultimopatronIndex = -1;

volatile uint32_t currentStep = 0; // Posición actual del paso (96 PPQN)
const int seqLength = 384; // Longitud de la secuencia (en pasos)
const int screenWidth = 117;
volatile uint32_t triangleX = 10; // Posición actual del triángulo
const int lineY = 42; // Y de la línea horizontal

unsigned long lastUpdateTime = 0; // Última vez que se actualizó el triángulo
const unsigned long updateInterval = 190; // Intervalo en ms para actualizar

ABRSequencer sequencer(pinA1Re, pinB1Re, pinFw, 120, &triangleX);

void setup() {
    // Inicialización de ambas pantallas
    u8g2.begin();
    u8g2.setContrast(10);
    u8g2.enableUTF8Print();

    u8g2_1.begin();
    u8g2_1.setContrast(15);
    u8g2_1.enableUTF8Print();

    Serial.begin(9600);       // Serial para depuración
    while (!Serial) {
    }
    Serial.println("Iniciando depuración en USB Serial...");

    // Incialización del secuenciador
    sequencer.beginSequencer();
    Serial.println("Secuenciador MIDI Iniciado...");

    ptrnEncoder.setValue(1);

    // // zona temporal
    actualizarTexto(); // Calcula el ancho inicial del texto


    
}

void loop() {
  // Otras tareas, por ejemplo, ajustar el tempo o controlar la interfaz
  patronIndex = ptrnEncoder.update();
  tempo = sequencer.cheeckBpm();
  sequencer.update();

  // Pantalla 1
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_luBS10_tf);
    u8g2.drawFrame(0, 0, 128, 64);
    u8g2.setCursor(6, 25);
    u8g2.print(patronIndex);
    u8g2.drawLine(6, 35, 120, 35);
    u8g2.setCursor(14, 55);
    u8g2.print("BUENOS DÍAS");
  } while (u8g2.nextPage());

  // Pantalla 2
  // Detecta si patronIndex cambió
  if (patronIndex != ultimopatronIndex) {
    ultimopatronIndex = patronIndex;
    actualizarTexto(); // Recalcula el ancho del texto actual

    // Reinicia los valores para la animación si es necesario
    xOffset = 0;
    estadoActual = ESPERA_INICIAL;
    tiempoAnterior = millis(); // Reinicia el temporizador
  }
  unsigned long tiempoActual = millis();

  // Verifica si el texto requiere animación
  bool requiereAnimacion = strlen(patrones[patronIndex]) > 11;

  // Si el texto requiere animación, utiliza la máquina de estados
  if (requiereAnimacion) {

    switch (estadoActual) {
      case ESPERA_INICIAL:
        if (tiempoActual - tiempoAnterior >= intervaloEsperaInicial) {
          tiempoAnterior = tiempoActual;
          estadoActual = MOVIENDO; // Cambia al estado de movimiento
        }
        break;

      case MOVIENDO:
        if (tiempoActual - tiempoAnterior >= intervaloDesplazamiento) {
          tiempoAnterior = tiempoActual;
          xOffset--; // Desplaza el texto hacia la izquierda

          if (xOffset < -(anchoTexto - anchoPantalla)) { // Si el texto se ha desplazado completamente
            estadoActual = ESPERA_FINAL; // Cambia al estado de espera final
            tiempoAnterior = tiempoActual; // Reinicia el temporizador para la espera final
          }
        }
        break;

      case ESPERA_FINAL:
        if (tiempoActual - tiempoAnterior >= intervaloEsperaFinal) {
          tiempoAnterior = tiempoActual;
          estadoActual = REINICIO; // Cambia al estado de reinicio
        }
        break;

      case REINICIO:
        xOffset = 0; // Regresa el texto al estado inicial
        estadoActual = ESPERA_INICIAL; // Cambia al estado de espera inicial
        tiempoAnterior = tiempoActual; // Reinicia el tiempo para la espera inicial
        break;
    }
  } else {
    // Si no requiere animación, calcula la posición centrada
    xOffset = (anchoPantalla - anchoTexto) / 2;
  }

  u8g2_1.firstPage();
  do {
    u8g2_1.setFont(u8g2_font_crox4t_tf); // opción má acertada


    // NOMBRE DEL PATRÓN (centrado o deslizante)
    u8g2_1.setCursor(xOffset, 35); // Cambia "55" si necesitas ajustar la posición vertical
    u8g2_1.print(patrones[patronIndex]);

    // número del patrón
    u8g2_1.setFont(u8g2_font_luBS18_tf);
    u8g2_1.setCursor(-1, 18);
    char buffer[4]; // 3 dígitos + terminador nulo
    // Formateamos el número con ceros a la izquierda
    snprintf(buffer, sizeof(buffer), "%03ld", patronIndex);
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
      triangleX, lineY,    // Vértice superior
      triangleX - 3, lineY - 3,    // Esquina inferior izquierda
      triangleX + 3, lineY - 3     // Esquina inferior derecha
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
    u8g2_1.print(tempo);
    // u8g2_1.drawLine(4, 64, 20, 127)





    // u8g2_1.setFont(u8g2_font_Born2bSportyV2_tr);
    // u8g2_1.setCursor(4, 60);
    // u8g2_1.print("01 - 1");


  } while (u8g2_1.nextPage());

}