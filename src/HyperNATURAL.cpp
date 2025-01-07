#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>
#include <SerialFlash.h>
#include "HyperNATURAL.h"

AudioPlaySdWav           playWav1;
AudioOutputI2S           audioOutput;

AudioConnection          patchCord1(playWav1, 0, audioOutput, 0);
AudioConnection          patchCord2(playWav1, 1, audioOutput, 1);
AudioControlSGTL5000     sgtl5000_1;

// Use these with the Teensy 3.5 & 3.6 & 4.1 SD card
#define SDCARD_CS_PIN    BUILTIN_SDCARD

HyperNATURAL::HyperNATURAL() {

}

void HyperNATURAL::initializeSG() {
  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(10);
  /*
  10 bloques
  cada bloque tiene un tamaño fijo de 128 muestras.
  al trabajar con audio de 44.1kHz cada muestra ocupa 16 bits (2 bytes)
  por tanto cada bloque de memoria tiene un tamaño fijo de 128 x 2 bytes (muestrta) = 256 bytes por bloque
  Este buffer se almacena en la RAM2, y con prueba y error se puede asignar un buffer de 1500 muestras
  consumiendo el 76% de la memoria.
  Teóricamente con 1500 bloques y considerando que cada audio consume 3 bloques se tiene una polifonía máxima
  de 1500 / 3 = 500 Sonidos simultáneos.

  A este cálculo se le debe incrementar el consumo de bloques por efectos, conexiones, etc.
  Serial.println(AudioMemoryUsageMax());
  */

  // Comment these out if not using the audio adaptor board.
  // This may wait forever if the SDA & SCL pins lack
  // pullup resistors
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.5);
  Serial.println("Iniciando depuración en USB Serial...");

  if (!(SD.begin(SDCARD_CS_PIN))) {
      // stop here, but print a message repetitively
      while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
      }
  }
}

void HyperNATURAL::playFile(const char *filename)
{
  // Serial.print("Playing file: ");
  // Serial.println(filename);

  // Start playing the file.  This sketch continues to
  // run while the file plays.
  playWav1.play(filename);

  // A brief delay for the library read WAV info
  // delay(25);

  // Simply wait for the file to finish playing.
  // while (playWav1.isPlaying()) {
    // uncomment these lines if you audio shield
    // has the optional volume pot soldered
    //float vol = analogRead(15);
    //vol = vol / 1024;
    // sgtl5000_1.volume(vol);
  // }
}

void HyperNATURAL::loop(volatile bool &playSnare) {
  if (playSnare) {
    playFile("/samples/cencerro/lprock5.wav");  // filenames are always uppercase 8.3 format
    playSnare = false;
  }
    // Serial.print("a");
    // delay(500);
}