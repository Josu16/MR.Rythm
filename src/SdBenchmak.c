#include <Arduino.h>
#include <SdFat.h>

// Usa SDIO para la Teensy 4.1
SdFat sd;

const char *TEST_FILE = "bench.dat"; // Archivo de prueba
const int BLOCK_SIZES[] = {512, 1024, 2048, 4096}; // Tamaños de bloque a probar
const int FILE_SIZES[] = {100 * 1024, 150 * 1024, 500 * 1024, 1024 * 1024, 2 * 1024 * 1024, 5 * 1024 * 1024, 10 * 1024 * 1024}; // Tamaños de archivo

const int NUM_TESTS = 5; // Número de repeticiones por prueba
uint8_t buffer[4096]; // Buffer máximo para bloques de hasta 4096 bytes

void testWriteSpeed(int fileSize, int blockSize, float &avgSpeed) {
  float totalSpeed = 0;

  for (int test = 0; test < NUM_TESTS; test++) {
    FsFile file = sd.open(TEST_FILE, O_WRONLY | O_CREAT | O_TRUNC);
    if (!file) {
      Serial.println("Error al crear el archivo de prueba.");
      return;
    }

    uint32_t start = millis();
    for (uint32_t i = 0; i < fileSize / blockSize; i++) {
      if (file.write(buffer, blockSize) != blockSize) {
        Serial.println("Error al escribir en la tarjeta SD.");
        file.close();
        return;
      }
    }
    uint32_t elapsed = millis() - start;

    file.close();

    float speed = (float)fileSize / (1024.0 * 1024.0) / (elapsed / 1000.0);
    totalSpeed += speed;
  }

  avgSpeed = totalSpeed / NUM_TESTS;
}

void testReadSpeed(int fileSize, int blockSize, float &avgSpeed) {
  float totalSpeed = 0;

  for (int test = 0; test < NUM_TESTS; test++) {
    FsFile file = sd.open(TEST_FILE, O_RDONLY);
    if (!file) {
      Serial.println("Error al abrir el archivo de prueba para lectura.");
      return;
    }

    uint32_t start = millis();
    for (uint32_t i = 0; i < fileSize / blockSize; i++) {
      if (file.read(buffer, blockSize) != blockSize) {
        Serial.println("Error al leer de la tarjeta SD.");
        file.close();
        return;
      }
    }
    uint32_t elapsed = millis() - start;

    file.close();

    float speed = (float)fileSize / (1024.0 * 1024.0) / (elapsed / 1000.0);
    totalSpeed += speed;
  }

  avgSpeed = totalSpeed / NUM_TESTS;
}

void testRandomReadSpeed(int fileSize, int blockSize, float &avgSpeed, int &avgIOPS) {
  float totalSpeed = 0;
  int totalIOPS = 0; // Variable para contar las operaciones por segundo
  const int NUM_RANDOM_READS = 1000; // Número de lecturas aleatorias por prueba

  for (int test = 0; test < NUM_TESTS; test++) {
    FsFile file = sd.open(TEST_FILE, O_RDONLY);
    if (!file) {
      Serial.println("Error al abrir el archivo de prueba para lectura aleatoria.");
      return;
    }

    uint32_t start = millis();
    for (int i = 0; i < NUM_RANDOM_READS; i++) {
      uint32_t randomPosition = random(0, fileSize / blockSize) * blockSize;
      if (!file.seek(randomPosition)) {
        Serial.println("Error al buscar posición aleatoria en el archivo.");
        file.close();
        return;
      }
      if (file.read(buffer, blockSize) != blockSize) {
        Serial.println("Error al leer de la tarjeta SD en posición aleatoria.");
        file.close();
        return;
      }
    }
    uint32_t elapsed = millis() - start;

    file.close();

    // Calcular velocidad en MB/s
    float speed = (float)(NUM_RANDOM_READS * blockSize) / (1024.0 * 1024.0) / (elapsed / 1000.0);
    totalSpeed += speed;

    // Calcular IOPS: número de operaciones dividido por el tiempo en segundos
    totalIOPS += NUM_RANDOM_READS / (elapsed / 1000.0);
  }

  avgSpeed = totalSpeed / NUM_TESTS;
  avgIOPS = totalIOPS / NUM_TESTS;
}

void runBenchmark() {
  Serial.println("Iniciando benchmark para tarjeta SD...");

  for (int sizeIndex = 0; sizeIndex < sizeof(FILE_SIZES) / sizeof(FILE_SIZES[0]); sizeIndex++) {
    int fileSize = FILE_SIZES[sizeIndex];
    Serial.printf("\nPruebas para archivo de %d KB\n", fileSize / 1024);

    for (int blockIndex = 0; blockIndex < sizeof(BLOCK_SIZES) / sizeof(BLOCK_SIZES[0]); blockIndex++) {
      int blockSize = BLOCK_SIZES[blockIndex];
      Serial.printf("  Tamaño de bloque: %d bytes\n", blockSize);

      float writeSpeed, readSpeed, randomReadSpeed;
      int randomReadIOPS;

      testWriteSpeed(fileSize, blockSize, writeSpeed);
      testReadSpeed(fileSize, blockSize, readSpeed);
      testRandomReadSpeed(fileSize, blockSize, randomReadSpeed, randomReadIOPS);

      Serial.printf("    Vel. Escritura: %.2f MB/s\n", writeSpeed);
      Serial.printf("    Vel. Lectura:   %.2f MB/s\n", readSpeed);
      Serial.printf("    Lectura Aleat.: %.2f MB/s (%d IOPS)\n", randomReadSpeed, randomReadIOPS);
    }
  }

  // Limpia el archivo de prueba al final
  sd.remove(TEST_FILE);
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    // Espera al monitor serie
  }

  Serial.println("Inicializando SD...");
  if (!sd.begin(SdioConfig())) {
    Serial.println("Error al inicializar la tarjeta SD.");
    while (true);
  }
  Serial.println("Tarjeta SD inicializada correctamente.");
  
  runBenchmark();
  Serial.println("\nBenchmark completado.");
}

void loop() {
  // No se necesita código en el loop
}
