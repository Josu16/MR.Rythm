#ifndef CONTROL_H
#define CONTROL_H

#include <arduino.h>

#include "RotaryEncoder.h"

const int pinA1Re = 2; // TODO: cambiar todas las declaraciones de pines a defines
const int pinB1Re = 3;
const int pinFw = 41;

const int pinA2Re = 4;
const int pinB2Re = 5;
const int pinLoockTempo = 40;

const int pinA3Re = 6;
const int pinB3Re = 7;
const int pinLoockVariation = 39;

// switches

// TEMPORALES, ESTAS SE MOVERÁN PRONTO

class Control {
    private:
        // Rotary Encoders
        RotaryEncoder bpmRE;
        // long positionBpmRe = 0;

        RotaryEncoder ptrnRE;
        // long positionPtrnRE = 0;

        RotaryEncoder variantRE;
        // long positionVariantRE = 1;


        volatile bool footswitchState;
        volatile bool footswitchChanged;
        volatile unsigned long lastDebounceTime = 0;
        const unsigned long debounceDelay = 200;  // Tiempo de debounce en milisegundos

        volatile bool loockTempoTriggered;
        bool loockTempoState;
        volatile bool loockTempoChanged;
        unsigned long lastDebounceTimeTempo = 0;
        const unsigned long debounceDelayTempo = 400;  // Tiempo de debounce en milisegundos

        // Pines de hardware
        const int BUTTON_PIN = 23;   // Pin al que está conectado el botón
        const int LED_PIN = 15;     // Pin para un LED de prueba

        // Constantes de máscara para el debouncer
        static const uint8_t MASK = 0b11000111;    // Máscara para detectar el patrón de "presionado"

        // Variables globales
        uint8_t button_history = 0;   // Historial del estado del botón
        uint8_t press_count = 1;      // Contador de pulsaciones
        uint8_t release_count = 0;    // Contador de liberaciones

        // Referencia estática a la instancia actual
        static Control* instance;

    public:
        Control(volatile long tempo, volatile long pattern);

        void refreshControls();

        // lectura de controles
        long readBpm();
        long readPtrn();
        long readVariant();
        bool checkForFootswitch();
        bool cheeckForLoockTempo();
        
        // Seteo de controles
        void setBpm(int newValue);
        void setPtrn(int newValue);
        void setVariant(int newValue);
        void setMaxVariant(int newValue);

        // MANEJO DE INTERRUPCIONES
        // footswitches
        static void fwISR();
        void handleFootswitchInterrupt();
        
        static void loockTempoISR();
        void handleLoockTempooInterrupt();

        uint8_t read_button(void);
        void update_button(uint8_t *button_history);
        uint8_t is_button_pressed(uint8_t *button_history);
        uint8_t is_button_released(uint8_t *button_history);
        uint8_t is_button_down(uint8_t *button_history);
        uint8_t is_button_up(uint8_t *button_history);
        bool tmpFootSwitch();
};

#endif