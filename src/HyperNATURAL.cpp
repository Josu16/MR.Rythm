#include <Arduino.h>

#include "HyperNATURAL.h"

// Crear instancias separadas de generadores de onda seno
AudioSynthWaveformSine sine1;
AudioSynthWaveformSine sine2;
AudioSynthWaveformSine sine3;
AudioSynthWaveformSine sine4;

// Salida de audio quad (4 canales)
AudioOutputI2SQuad i2s_quad2;

// Conectar cada generador a un canal específico
AudioConnection patchCord1(sine1, 0, i2s_quad2, 0); // Canal 1
AudioConnection patchCord2(sine2, 0, i2s_quad2, 1); // Canal 2
AudioConnection patchCord3(sine3, 0, i2s_quad2, 2); // Canal 3
AudioConnection patchCord4(sine4, 0, i2s_quad2, 3); // Canal 4

HyperNATURAL::HyperNATURAL() {
    AudioMemory(12);

    // Configurar frecuencia y amplitud para cada canal
    sine1.frequency(440); // Frecuencia para Canal 1 DERECHO
    sine1.amplitude(0.5); // Amplitud para Canal 1 (-6dB) 

    sine2.frequency(880); // Frecuencia para Canal 2 IZQUIERDO
    sine2.amplitude(0.5); // Amplitud para Canal 2 (-6dB)

    sine3.frequency(1760); // Frecuencia para Canal 3 IZQUIERDO
    sine3.amplitude(0.5); // Amplitud para Canal 3 (-6dB)

    sine4.frequency(3520); // Frecuencia para Canal 4 DERECHO
    sine4.amplitude(0.5); // Amplitud para Canal 4 (-6dB)
}

void HyperNATURAL::loop() {
    // Serial.print("a");
}