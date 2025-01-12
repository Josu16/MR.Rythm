# Usa una imagen base de Ubuntu
FROM ubuntu:latest

# Establece el directorio de trabajo
WORKDIR /usr/src/app

# Instala dependencias necesarias, incluyendo git y python 3.12
RUN apt-get update && apt-get install -y \
    git \
    python3 \
    python3-venv \
    # python3.12-distutils \ Se eliminó a partir de la versión 3.10
    python3-pip \
    && apt-get clean

# Asegúrate de que Python 3.12 esté disponible como 'python'
RUN ln -s /usr/bin/python3 /usr/bin/python

# Establece el volumen donde se montará el código fuente
VOLUME ["/usr/src/app", "/dev/bus/usb"]

# Exponer el puerto necesario
EXPOSE 4444

# Comando por defecto que es el mismo que el comando original: mantiene el contenedor corriendo
CMD tail -f /dev/null
