#include "Control.h"

Control* Control::instance = nullptr;

Control::Control(volatile long tempo, volatile long pattern)
    :
    bpmRE(pinA1Re, pinB1Re, tempo, 20, 250),
    ptrnRE(pinA2Re, pinB2Re, pattern, 1, 120), // TODO: revisar el parámetro long. corrgir.
    variantRE(pinA3Re, pinB3Re, 1, 1, 5) // TODO: revisar el parámetro long. corrgir.
{
    // FootSwitch
    pinMode(pinFw, INPUT_PULLUP);
    footswitchChanged = false;
    footswitchState = HIGH;
    lastDebounceTime = 0;
    attachInterrupt(digitalPinToInterrupt(pinFw), fwISR, CHANGE);

    // LoockTempo
    pinMode(pinLoockTempo, INPUT_PULLUP);
    loockTempoState = false;
    loockTempoChanged = false;
    attachInterrupt(digitalPinToInterrupt(pinLoockTempo), loockTempoISR, RISING);


    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
    instance = this;
}

long Control::readBpm() {
    return bpmRE.getPosition();
}

void Control::setBpm(int newValue) {
    bpmRE.setPosition(newValue);
}

long Control::readPtrn() {
    return ptrnRE.getPosition();
}

void Control::setPtrn(int newValue) {
    ptrnRE.setPosition(newValue);
}

void Control::setVariant(int newValue) {
    variantRE.setPosition(newValue);
}

void Control::setMaxVariant(int newValue) {
    variantRE.setMax(newValue);
}

long Control::readVariant() {
    return variantRE.getPosition();
} 

void Control::fwISR() {
    if (instance) {
        instance->handleFootswitchInterrupt();
    }
}
void Control::handleFootswitchInterrupt() {
    footswitchState = digitalRead(pinFw);
    footswitchChanged = true;
}

void Control::loockTempoISR() {
    if (instance) {
        instance->handleLoockTempooInterrupt();
    }
}

// REVISAR ESTA FUNCIÓN
/*
Cuando se recibe una interrupción, esta función entra muchas veces por presión del botón, es importante
tener un mecanismo de tratamiento de interrumpir tantas veces, porque esto puede entrar en conflicto 
con la interrupción más importante que es el timer.
*/
void Control::handleLoockTempooInterrupt() {
        loockTempoTriggered = true;
}

bool Control::checkForFootswitch() {
    if (footswitchChanged) {
        unsigned long currentTime = millis();
        if ((currentTime - lastDebounceTime) > debounceDelay) {
            lastDebounceTime = currentTime; // Actualizar el tiempo de debounce
            footswitchChanged = false;     // Reiniciar la marca de cambio
            return (footswitchState == LOW); // Devuelve si el footswitch está activo
        }
    }
    return false; // No hay cambio válido
}

bool Control::cheeckForLoockTempo() {
    if (loockTempoTriggered) {
        unsigned long currentTime = millis();
        if ((currentTime - lastDebounceTimeTempo) > debounceDelayTempo) {
            lastDebounceTime = currentTime;
            loockTempoState = !loockTempoState;
        }
        loockTempoTriggered = false;
    }
    return loockTempoState;
}

// REVISIÓN DE FOOTSWITCHES
/* 
Sección temporal:
Esta parte tiene que optimizarse (o desecharse) cuando se tenga el 74HC14 Schmitt Trigger
Por ahora la lectura del botón más importante (el incio y detención de secuencia), está
resuelto por software con un código de deboucing avanzado obtenido de:
https://hackaday.com/2015/12/10/embed-with-elliot-debounce-your-noisy-buttons-part-ii/
*/

// Función para leer el estado actual del botón
uint8_t Control::read_button(void) {
  return digitalRead(BUTTON_PIN) == LOW ? 1 : 0; // Activo en bajo
}

// Función para actualizar el historial del botón
void Control::update_button(uint8_t *button_history) {
  *button_history = (*button_history << 1) | read_button();
}
// Función para detectar si el botón ha sido presionado
uint8_t Control::is_button_pressed(uint8_t *button_history) {
  uint8_t pressed = 0;
  if ((*button_history & MASK) == 0b00000111) { // Patrón detectado
    pressed = 1;
    *button_history = 0b11111111; // Reinicia el historial al estado "presionado"
  }
  return pressed;
}

// Función para detectar si el botón ha sido liberado
uint8_t Control::is_button_released(uint8_t *button_history) {
  uint8_t released = 0;
  if ((*button_history & MASK) == 0b11000000) { // Patrón de liberación detectado
    released = 1;
    *button_history = 0b00000000; // Reinicia el historial al estado "liberado"
  }
  return released;
}

// Función para comprobar si el botón está en el estado "presionado"
uint8_t is_button_down(uint8_t *button_history) {
  return (*button_history == 0b11111111);
}

// Función para comprobar si el botón está en el estado "liberado"
uint8_t is_button_up(uint8_t *button_history) {
  return (*button_history == 0b00000000);
}

bool Control::tmpFootSwitch() {
    bool stateBtn = false;
    // Actualiza el historial del botón
    update_button(&button_history);

    // Detecta un evento de botón presionado
    if (is_button_pressed(&button_history)) {
        release_count++;
        Serial.print("Botón liberado: ");
        Serial.println(release_count);
        digitalWrite(LED_PIN, LOW); // Apaga el LED como ejemplo
    }

    // Detecta un evento de botón liberado
    if (is_button_released(&button_history)) {
        press_count++;
        Serial.print("Botón presionado: ");
        Serial.println(press_count);
        digitalWrite(LED_PIN, HIGH); // Enciende el LED como ejemplo
        stateBtn = true;
    }

    return stateBtn;
    }