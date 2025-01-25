## Hyper NATURAL Sound Generator

Esta sección del código contiene al módulo generador de sonidos. Su único objetivo es gestionar el disparo de samples que correspondan dados los mensajes que mande el secuenciador. Está basado en una Raspberry PI Zero 2W (Arm Cortex-A53 4 núcleos a 1GHz con 512 MB de SDRAM).

La necesidad de un procesamiento más exhaustivo es por los problemas de polifonía encontrados en la teensy 4.1, con una memoria microsd se alcanzaron velocidades deficientes para disparo en tiempo real de samples, por tanto, se explorará la capacidad de una raspberry para esta tarea.

Como los objetivos son muy precisos y cruciales, la raspberry no funje como un microordenador, es un sistema de tiempo real programado en baremetal con el framework Circle.

https://github.com/rsta2/circle

### Instalación:

https://circle-rpi.readthedocs.io/en/47.0/getting-started.html

### 