# # Usa una imagen base de Ubuntu
# FROM ubuntu:latest

# # RUN "wget 'https://developer.arm.com/-/media/Files/downloads/gnu/14.2.rel1/binrel/arm-gnu-toolchain-14.2.rel1-x86_64-aarch64-none-elf.tar.xz'"
# # wget -O arm-toolchain.tar.xz https://developer.arm.com/-/media/Files/downloads/gnu/14.2.rel1/binrel/arm-gnu-toolchain-14.2.rel1-x86_64-aarch64-none-elf.tar.xz
# # # Instala herramientas necesarias
# # RUN apk add --no-cache \
# #     build-base \
# #     gcc-arm-none-eabi \
# #     g++-arm-none-eabi \
# #     cmake \
# #     git \
# #     python3 \
#         xz-utils \
#         nano
#         #     libstdc++ \
#         #     newlib-arm-none-eabi
        
# # Configura el directorio de trabajo
# WORKDIR /workspace

# RUN "wget -O arm-13.2-toolchain.tar.xz 'https://developer.arm.com/-/media/Files/downloads/gnu/13.2.rel1/binrel/arm-gnu-toolchain-13.2.rel1-x86_64-aarch64-none-elf.tar.xz?rev=a05df3001fa34105838e6fba79ee1b23&hash=D63F63D13F01626D207019956E7122B5'"




# Usa Ubuntu 22.04 LTS como base
FROM ubuntu:22.04

# Establece un entorno no interactivo para evitar prompts durante la instalación
ENV DEBIAN_FRONTEND=noninteractive

# Instala herramientas necesarias
RUN apt update && apt install -y \
    wget \
    git \
    xz-utils \
    build-essential \
    libstdc++6 \
    libc6 \
    && apt clean

# Descarga el toolchain ARM
RUN wget -O /tmp/arm-13.2-toolchain.tar.xz "https://developer.arm.com/-/media/Files/downloads/gnu/13.2.rel1/binrel/arm-gnu-toolchain-13.2.rel1-x86_64-aarch64-none-elf.tar.xz?rev=a05df3001fa34105838e6fba79ee1b23&hash=D63F63D13F01626D207019956E7122B5"

# Descomprime el toolchain
RUN mkdir -p /opt/arm-toolchain && \
    tar -xvf /tmp/arm-13.2-toolchain.tar.xz -C /opt/arm-toolchain && \
    rm /tmp/arm-13.2-toolchain.tar.xz

# Agrega el toolchain al PATH de manera permanente
ENV PATH="/opt/arm-toolchain/arm-gnu-toolchain-13.2.Rel1-x86_64-aarch64-none-elf/bin:$PATH"

# Configura un alias para el compilador
# RUN echo 'alias gccarm="aarch64-none-elf-gcc"' >> /etc/bash.bashrc

# Establece el directorio de trabajo
RUN mkdir -p /workspace

WORKDIR /workspace

# RUN mkdir -p /workspace/SoundGenerator

# Comando por defecto para el contenedor
CMD ["/bin/bash"]
