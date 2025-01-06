// RotaryEncoder.h
#ifndef HYPERNATURAL_H
#define HYPERNATURAL_H

#include <Audio.h>
#include "pins.h"

/*
Para la conexión del PCM 5102 se utilizaron las siguientes referencias de conexión:
https://forum.pjrc.com/index.php?threads/pcm5012-i2s-dac-with-teensy-4-1.68625/

https://forum.pjrc.com/index.php?threads/teensy-4-1-dac-pcm1502.76000/
"Sobre este último solo se tomó la conexión de 3v3 a XMT, los GNDs del comentario 4
no son imprescindibles"

Para conectar multiples DACs
https://forum.pjrc.com/index.php?threads/teensy-4-with-two-pcm5102-dac.68280/
*/

class HyperNATURAL {
public:
    HyperNATURAL();
    void initializeSG();
    void playFile(const char *filename);
    void loop(volatile bool &playSnare);

private:

};

#endif // HYPERNATURAL