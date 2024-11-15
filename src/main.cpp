#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <IntervalTimer.h>
#include <IntervalTimer.h>

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

// Estructura para eventos MIDI
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
  digitalWrite(14, HIGH);  // Enciende el LED
  delay(1000);                  // Espera 500 ms
  digitalWrite(14, LOW);   // Apaga el LED
  delay(1000);                  // Espera 500 ms
}
