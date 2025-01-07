#include "InternalFlashFS.h"

InternalFlashFS::InternalFlashFS() {
    diskSize = PROG_FLASH_SIZE;

    // checks that the LittFS program has started with the disk size specified
    if (!mr9Fs.begin(diskSize)) {
        Serial.printf("Error starting %s\n", "PROGRAM FLASH DISK");
        while (1) {
        // Error, so don't do anything more - stay stuck here
        }
    }

    // PROPÓSITOS DE DESARROLLO (BORRADO DE TODOS LOS ARCHIVOS)
    // mr9Fs.lowLevelFormat();  // performs a quick format of the created di
}

void InternalFlashFS::getPattern(uint16_t numberPattern, Pattern &requiredPattern) {
    char fileName[10];  // Arreglo para almacenar el nombre del archivo
    // Componer el nombre del archivo dinámicamente
    sprintf(fileName, "%u.mr9", numberPattern);
    patternFile = mr9Fs.open(fileName, FILE_READ);
    if (patternFile) {
        patternFile.read((uint8_t*)&requiredPattern, sizeof(Pattern));  // Lee los bytes y los almacena en la estructura
        patternFile.close();
        Serial.println("Datos cargados correctamente.");
    } else {
        Serial.println("Error al abrir el archivo para lectura.");
        // validFunc = false;
    }
    printSongDetails(requiredPattern);
}

bool InternalFlashFS::savePattern(const Pattern &currentPattern) {
    // printDirectory(mr9Fs);
    bool validFunc = true;

    char fileName[10];  // Arreglo para almacenar el nombre del archivo
    // Componer el nombre del archivo dinámicamente
    sprintf(fileName, "%u.mr9", currentPattern.number);

    if (mr9Fs.exists(fileName)) {
        Serial.printf("Patron %s ya existe.\n", fileName);

        // PROPÓSITOS DE DESARROLLO:
        // if (mr9Fs.remove(fileName)) {
        //     Serial.println("Archivo borrado exitosamente.");
        // } else {
        //     Serial.println("Error al intentar borrar el archivo.");
        //     validFunc = false;
        // }
    } else {
        Serial.printf("Creando patron %s ...\n", fileName);

        // Crear y escribir en el archivo
        patternFile = mr9Fs.open(fileName, FILE_WRITE);
        if (patternFile) {

            patternFile.write((uint8_t*)&currentPattern, sizeof(Pattern));

            patternFile.close();
            Serial.println("Archivo creado exitosamente.");
        } else {
            Serial.println("Error al crear el archivo.");
            validFunc = false;
        }
    }
    printSongDetails(currentPattern);



    // Pattern patronTemporal = getPattern(1);

    // printSongDetails(patronTemporal);

    // while(1) {

    // }
    return validFunc;
}

void InternalFlashFS::listInternalMemoryPatterns() {
    Serial.println("Patrones en memoria interna: ");
    printDirectory(mr9Fs);
    Serial.print("\n Space Used = ");
    Serial.println(mr9Fs.usedSize());
    Serial.print("Filesystem Size = ");
    Serial.println(mr9Fs.totalSize());
}

void InternalFlashFS::printDirectory(FS &fs) {
  Serial.println("Directorio\n---------");
  printDirectory(fs.open("/"), 0);
  Serial.println();
}

void InternalFlashFS::printDirectory(File dir, int numSpaces) {
   while(true) {
     File entry = dir.openNextFile();
     if (! entry) {
       //Serial.println("** no more files **");
       break;
     }
     printSpaces(numSpaces);
     Serial.print(entry.name());
     if (entry.isDirectory()) {
       Serial.println("/");
       printDirectory(entry, numSpaces+2);
     } else {
       // files have sizes, directories do not
       printSpaces(36 - numSpaces - strlen(entry.name()));
       Serial.print("  ");
       Serial.print(entry.size(), DEC);
       DateTimeFields datetime;
       if (entry.getModifyTime(datetime)) {
         printSpaces(4);
         printTime(datetime);
       }
       Serial.println();
     }
     entry.close();
   }
}

void InternalFlashFS::printSpaces(int num) {
  for (int i = 0; i < num; i++) {
    Serial.print(" ");
  }
}

void InternalFlashFS::printTime(const DateTimeFields tm) {
  const char *months[12] = {
    "Enero","Febrero","Marzo","Abril","Mayo","Junio",
    "Julio","Agosto","Septiembre","Octubre","Noviembre","Diciembre"
  };
  if (tm.hour < 10) Serial.print('0');
  Serial.print(tm.hour);
  Serial.print(':');
  if (tm.min < 10) Serial.print('0');
  Serial.print(tm.min);
  Serial.print("  ");
  Serial.print(tm.mon < 12 ? months[tm.mon] : "???");
  Serial.print(" ");
  Serial.print(tm.mday);
  Serial.print(", ");
  Serial.print(tm.year + 1900);
}

// Función para imprimir los detalles de la estructura Song
void InternalFlashFS::printSongDetails(const Pattern& s) {
    Serial.println("---------------------Song Details:");
    
    // Imprimir los campos de la estructura
    Serial.print("Track Name: ");
    Serial.println(s.tackName);
    
    Serial.print("Pattern Number: ");
    Serial.println(s.number);
    
    Serial.print("Tempo: ");
    Serial.println(s.tempo);
    
    Serial.print("Time Signature: ");
    Serial.print(s.numerator);
    Serial.print("/");
    Serial.println(s.denominator);
    
    Serial.print("Measures: ");
    Serial.println(s.measures);
    
    Serial.print("Total Ticks: ");
    Serial.println(s.totalTicks);
    
    Serial.print("Number of Events: ");
    Serial.println(s.numEvents);
    
    Serial.print("Total Variants: ");
    Serial.println(s.totalVariants);
    
    // Imprimir eventos por variante
    for (int i = 0; i < 5; i++) {
        Serial.print("Events by Variant ");
        Serial.print(i);
        Serial.print(": ");
        Serial.println(s.eventsByVariant[i]);
    }
    
    // // Imprimir algunos de los eventos (solo los primeros para evitar mucho volcado)
    // Serial.println("First 5 events:");
    // for (int i = 0; i < 5 && i < s.numEvents; i++) {
    //     MidiEvent event = s.events[0][i];  // Solo imprime eventos de la primera variante
    //     Serial.print("Event ");
    //     Serial.print(i);
    //     Serial.print(" - Tick: ");
    //     Serial.print(event.tick);
    //     Serial.print(", Type: ");
    //     Serial.print(event.type);
    //     Serial.print(", Data: ");
    //     for (int j = 0; j < 3; j++) {
    //         Serial.print(event.note);
    //         Serial.print(" ");
    //         Serial.print(event.velocity);
    //         Serial.print(" ");
    //     }
    //     Serial.println();
    // }
}