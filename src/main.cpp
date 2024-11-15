#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <IntervalTimer.h>
#include <IntervalTimer.h>

// ----------------------------- SEQUENCER DECLARATION VARIABLES -----------------------------

IntervalTimer myTimer;
volatile uint32_t currentTick = 0;  // Contador de ticks
volatile uint16_t bpm = 120;        // Tempo de la secuencia

// Definimos los tipos de eventos MIDI
#define NOTE_ON  0x90
#define NOTE_OFF 0x80

// Definimos las notas de los instrumentos MIDI (ej. GM Drums)
#define KICK_DRUM 36
#define SNARE_DRUM 38
#define HI_HAT 42

// Estructura para secuencias de eventos MIDI
struct MidiEvent {
    uint8_t type;     // NOTE_ON o NOTE_OFF
    uint8_t note;     // Nota MIDI (ej. 36 = KICK_DRUM)
    uint8_t velocity; // Velocidad (ej. 100)
    uint32_t tick;    // Momento en ticks (96 PPQN)
};

// Ejemplo de ritmo de batería
MidiEvent drumPattern[] = {
    {NOTE_ON, KICK_DRUM, 100, 0},     // Bombo en el primer tick
    {NOTE_OFF, KICK_DRUM, 100, 48},   // Apagar bombo en el tick 48
    {NOTE_ON, SNARE_DRUM, 100, 96},   // Caja en el segundo cuarto de nota
    {NOTE_OFF, SNARE_DRUM, 100, 144}, // Apagar caja en el tick 144
    // {NOTE_ON, HI_HAT, 100, 0},        // Hi-hat al inicio
    // {NOTE_OFF, HI_HAT, 100, 24},      // Apagar hi-hat en el tick 24
    // {NOTE_ON, HI_HAT, 100, 48},       // Hi-hat en el segundo tick
    // {NOTE_OFF, HI_HAT, 100, 72},      // Apagar hi-hat en el tick 72
    // {NOTE_ON, HI_HAT, 100, 96},       // Hi-hat en el tercer tick
    // {NOTE_OFF, HI_HAT, 100, 120},     // Apagar hi-hat en el tick 120
    // {NOTE_ON, HI_HAT, 100, 144},      // Hi-hat en el cuarto tick
    // {NOTE_OFF, HI_HAT, 100, 168},     // Apagar hi-hat en el tick 168
};

// Configuración de variables para la reproducción
const size_t patternLength = sizeof(drumPattern) / sizeof(drumPattern[0]);
size_t eventIndex = 0;  // Índice del evento actual

// ----------------------------- END SEQUENCER DECLARATION VARIABLES -----------------------------


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
void onTimer() {
    // Esta función se ejecuta cada 5208 microsegundos

    // Verifica si el evento actual debe reproducirse en el tick actual
    if (eventIndex < patternLength && drumPattern[eventIndex].tick == currentTick) {
        MidiEvent event = drumPattern[eventIndex];
        
        // Enviar el evento MIDI en Serial7 (puerto MIDI)
        Serial7.write(event.type);
        Serial7.write(event.note);
        Serial7.write(event.velocity);

        // Avanza al siguiente evento
        eventIndex++;
    }
    currentTick++;  // Incrementa el contador de ticks

    // Reinicia el patrón después de 384 ticks (4/4 en 96 PPQN)
    if (currentTick >= 384) {
        currentTick = 0;  // Reinicia los ticks
        eventIndex = 0;   // Reinicia el índice de eventos
    }
}

// Función para actualizar el timer en función del BPM
void updateTimerInterval() {
    uint32_t interval = 60000000 / (bpm * 96); // Intervalo en microsegundos
    myTimer.begin(onTimer, interval);          // Actualiza el timer con el nuevo intervalo
}

void setup() {
    // Inicialización de ambas pantallas
    u8g2.begin();
    u8g2.setContrast(10);
    u8g2.enableUTF8Print();

    u8g2_1.begin();
    u8g2_1.setContrast(10);
    u8g2_1.enableUTF8Print();

    Serial7.begin(31250);     // Serial MIDI a 31,250 baudios
    Serial.begin(9600);       // Serial para depuración
    while (!Serial) {
    }
    Serial.println("Iniciando depuración en USB Serial...");

    // Inicia el timer para dispararse cada 5208 microsegundos (para 120 BPM a 96 PPQN)
    updateTimerInterval();
    Serial.println("Secuenciador MIDI Iniciado...");

    pinMode(14, OUTPUT);
}

void loop() {
    // Otras tareas, por ejemplo, ajustar el tempo o controlar la interfaz
    // // Pantalla 1
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_luBS10_tf);
    u8g2.drawFrame(0, 0, 128, 64);
    u8g2.setCursor(6, 25);
    u8g2.print("HOA mundo ¡");
    u8g2.drawLine(6, 35, 120, 35);
    u8g2.setCursor(14, 55);
    u8g2.print("BUENOS DÍAS");
  } while (u8g2.nextPage());
  digitalWrite(14, HIGH);  // Enciende el LED
  delay(1000);                  // Espera 1000 ms

  // Pantalla 2
  u8g2_1.firstPage();
  do {
    u8g2_1.setFont(u8g2_font_luBS10_tf);
    u8g2_1.drawFrame(0, 0, 128, 64);
    u8g2_1.setCursor(6, 25);
    u8g2_1.print(bpm);
    u8g2_1.drawLine(6, 35, 120, 35);
    u8g2_1.setCursor(14, 55);
    u8g2_1.print("BUENOS DÍAS");
  } while (u8g2_1.nextPage());
  digitalWrite(14, LOW);   // Apaga el LED
  delay(1000);                  // Espera 1000 ms
}
