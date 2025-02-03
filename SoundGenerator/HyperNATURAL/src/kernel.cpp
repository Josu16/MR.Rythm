//
// kernel.cpp
//
// Circle - A C++ bare metal environment for Raspberry Pi
// Copyright (C) 2014-2024  R. Stange <rsta2@o2online.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#define PARTITION	"umsd1-1"
#define FILENAME	"circle.txt"
#define NSAMPLES 3

// #define IO_BUFFER_SIZE 1024

#include "kernel.h"
#include "config.h"
#include <circle/sound/pwmsoundbasedevice.h>
#include <circle/sound/i2ssoundbasedevice.h>
#include <circle/sound/hdmisoundbasedevice.h>
#include <circle/sound/usbsoundbasedevice.h>
#include <circle/machineinfo.h>
#include <circle/util.h>
#include <circle/memory.h>
#include <assert.h>

#ifdef USE_VCHIQ_SOUND
	#include <vc4/sound/vchiqsoundbasedevice.h>
#endif

#if WRITE_FORMAT == 0
	#define FORMAT		SoundFormatUnsigned8
	#define TYPE		u8
	#define TYPE_SIZE	sizeof (u8)
	#define FACTOR		((1 << 7)-1)
	#define NULL_LEVEL	(1 << 7)
#elif WRITE_FORMAT == 1
	#define FORMAT		SoundFormatSigned16
	#define TYPE		s16
	#define TYPE_SIZE	sizeof (s16)
	#define FACTOR		((1 << 15)-1)
	#define NULL_LEVEL	0
#elif WRITE_FORMAT == 2
	#define FORMAT		SoundFormatSigned24
	#define TYPE		s32
	#define TYPE_SIZE	(sizeof (u8)*3)
	#define FACTOR		((1 << 23)-1)
	#define NULL_LEVEL	0
#endif

static const char FromKernel[] = "kernel";

static const char *wavMemory[3] = {
	"lprock6.wav",
	"snare.wav",
	"hat.wav"
};

struct WAVHeader {
    char chunkID[4];        // "RIFF"
    unsigned chunkSize;     // Tamaño total del archivo - 8 bytes
    char format[4];         // "WAVE"
    char subChunk1ID[4];    // "fmt "
    unsigned subChunk1Size; // Tamaño del subchunk "fmt" (16 para PCM)
    unsigned short audioFormat;  // Formato de audio (1 = PCM)
    unsigned short numChannels;  // Número de canales (1 = Mono, 2 = Estéreo)
    unsigned sampleRate;    // Frecuencia de muestreo (ej., 44100 Hz)
    unsigned byteRate;      // Bytes por segundo = SampleRate * NumChannels * BitsPerSample/8
    unsigned short blockAlign;   // BlockAlign = NumChannels * BitsPerSample/8
    unsigned short bitsPerSample; // Bits por muestra (16 bits = 2 bytes)
    char subChunk2ID[4];    // "data"
    unsigned subChunk2Size; // Tamaño de los datos de audio (en bytes)
};

CKernel::CKernel (void)
:	m_Screen (m_Options.GetWidth (), m_Options.GetHeight ()),
	m_Timer (&m_Interrupt),
	m_Logger (m_Options.GetLogLevel (), &m_Timer),
	m_I2CMaster (CMachineInfo::Get ()->GetDevice (DeviceI2CMaster), TRUE),
	m_USBHCI (&m_Interrupt, &m_Timer, FALSE),
#ifdef USE_VCHIQ_SOUND
	m_VCHIQ (CMemorySystem::Get (), &m_Interrupt),
#endif
	m_pSound (0)
	// m_VFO (&m_LFO)		// LFO modulates the VFO
{
	m_ActLED.Blink (5);	// show we are alive
}

CKernel::~CKernel (void)
{
}

boolean CKernel::Initialize (void)
{
	boolean bOK = TRUE;

	if (bOK)
	{
		bOK = m_Screen.Initialize ();
	}

	if (bOK)
	{
		// bOK = m_Serial.Initialize (115200);
		m_Serial.Initialize(9600, 8, 1, CSerialDevice::ParityNone); // ParityNone = 0
	}

	if (bOK)
	{
		// CDevice *pTarget = m_DeviceNameService.GetDevice (m_Options.GetLogDevice (), FALSE);
		// if (pTarget == 0)
		// {
		// 	pTarget = &m_Screen;
		// }

		bOK = m_Logger.Initialize (&m_Serial);
	}

	if (bOK)
	{
		bOK = m_Interrupt.Initialize ();
	}

	if (bOK)
	{
		bOK = m_Timer.Initialize ();
	}

	if (bOK)
	{
		bOK = m_I2CMaster.Initialize ();
	}

	if (bOK)
	{
		bOK = m_USBHCI.Initialize ();
	}

#ifdef USE_VCHIQ_SOUND
	if (bOK)
	{
		bOK = m_VCHIQ.Initialize ();
	}
#endif

	return bOK;
}

TShutdownMode CKernel::Run (void)
{
	m_Logger.Write (FromKernel, LogNotice, "Compile time: " __DATE__ " " __TIME__);

	// select the sound device
	const char *pSoundDevice = m_Options.GetSoundDevice ();
	if (strcmp (pSoundDevice, "sndpwm") == 0)
	{
		m_pSound = new CPWMSoundBaseDevice (&m_Interrupt, SAMPLE_RATE, CHUNK_SIZE);
	}
	else if (strcmp (pSoundDevice, "sndi2s") == 0)
	{
		m_pSound = new CI2SSoundBaseDevice (&m_Interrupt, SAMPLE_RATE, CHUNK_SIZE, FALSE,
						    &m_I2CMaster, DAC_I2C_ADDRESS);
	}
	else if (strcmp (pSoundDevice, "sndhdmi") == 0)
	{
		m_pSound = new CHDMISoundBaseDevice (&m_Interrupt, SAMPLE_RATE, CHUNK_SIZE);
	}
#if RASPPI >= 4
	else if (strcmp (pSoundDevice, "sndusb") == 0)
	{
		m_pSound = new CUSBSoundBaseDevice (SAMPLE_RATE);
	}
#endif
	else
	{
#ifdef USE_VCHIQ_SOUND
		m_pSound = new CVCHIQSoundBaseDevice (&m_VCHIQ, SAMPLE_RATE, CHUNK_SIZE,
					(TVCHIQSoundDestination) m_Options.GetSoundOption ());
#else
		m_pSound = new CPWMSoundBaseDevice (&m_Interrupt, SAMPLE_RATE, CHUNK_SIZE);
#endif
	}
	assert (m_pSound != 0);

	// initialize oscillators
	// m_LFO.SetWaveform (WaveformSine);
	// m_LFO.SetFrequency (10.0);

	// m_VFO.SetWaveform (WaveformSine);
	// m_VFO.SetFrequency (440.0);
	// m_VFO.SetModulationVolume (0.25);


	// APERTURA DE ARCHIVOS

	m_Logger.Write (FromKernel, LogNotice, "Compile time: " __DATE__ " " __TIME__);

	// Mount file system
	CDevice *pPartition = m_DeviceNameService.GetDevice (PARTITION, TRUE);
	if (pPartition == 0)
	{
		m_Logger.Write (FromKernel, LogPanic, "Partition not found: %s", PARTITION);
	}

	if (!m_FileSystem.Mount (pPartition))
	{
		m_Logger.Write (FromKernel, LogPanic, "Cannot mount partition: %s", PARTITION);
	}

	// Show contents of root directory
	TDirentry Direntry;
	TFindCurrentEntry CurrentEntry;
	unsigned nEntry = m_FileSystem.RootFindFirst (&Direntry, &CurrentEntry);
	for (unsigned i = 0; nEntry != 0; i++) // iterar hasta que el número de entrads sea igual a cero.
	{
		if (!(Direntry.nAttributes & FS_ATTRIB_SYSTEM)) // Si no es un archivo de sistema
		{
			CString FileName;
			FileName.Format ("%-14s", Direntry.chTitle);

			m_Screen.Write ((const char *) FileName, FileName.GetLength ());
			m_Logger.Write (FromKernel, LogNotice, "Nombre del archivo %s ", (const char *) FileName);
			m_Scheduler.MsSleep (100);
			// const char *mensaje =   FileName;
			// m_Serial.Write(mensaje, strlen(mensaje));

			if (i % 5 == 4) // Solo  es formateo de impresión.
			{
				m_Screen.Write ("\n", 1);
			}
		}

		nEntry = m_FileSystem.RootFindNext (&Direntry, &CurrentEntry);
	}
	m_Screen.Write ("\n", 1);

	// for (unsigned indexSample = 0; indexSample < NSAMPLES; indexSample++) { 
	// AQUI DEBE INICIAR EL CÍCLO
	// while (1) {
		WAVHeader header;

		unsigned sample = m_FileSystem.FileOpen(wavMemory[0]);

		if (sample == 0) { // será cero cuando no haya más archivos en el directorio
			m_Logger.Write (FromKernel, LogPanic, "No se pudo abrir: %s ", wavMemory[0]);
		}
		else {
			m_Logger.Write (FromKernel, LogNotice, "Abierto: %s ", wavMemory[0]);
			char wavHeader[44];
			unsigned nEntryFile = m_FileSystem.FileRead(sample, wavHeader, 44);
			if (nEntryFile == FS_ERROR) {
				m_Logger.Write (FromKernel, LogError, "Error al abrir el header");
			}
			else if (nEntryFile != sizeof(wavHeader))
			{
				m_Logger.Write(FromKernel, LogError, "El header del archivo WAV tiene un tamaño inesperado: %u bytes leídos, se esperaban %u bytes", nEntryFile, (unsigned)sizeof(wavHeader));
			}
			else {
				// Opcional: Validar que la estructura tenga el tamaño correcto (44 bytes)
				if (sizeof(WAVHeader) != 44)
				{
					m_Logger.Write(FromKernel, LogError, "El tamaño de la estructura WAVHeader es %u bytes, se esperaba 44 bytes", (unsigned)sizeof(WAVHeader));
				}

				// Mapear los datos leídos en nuestra estructura
				
				memcpy(&header, wavHeader, sizeof(header));

				// Convertir los campos de 4 bytes en cadenas nulas terminadas
				char chunkID[5], format[5], subChunk1ID[5], subChunk2ID[5];
				memcpy(chunkID, header.chunkID, 4);
				chunkID[4] = '\0';
				memcpy(format, header.format, 4);
				format[4] = '\0';
				memcpy(subChunk1ID, header.subChunk1ID, 4);
				subChunk1ID[4] = '\0';
				memcpy(subChunk2ID, header.subChunk2ID, 4);
				subChunk2ID[4] = '\0';

				// Imprimir toda la información del header
				m_Logger.Write(FromKernel, LogNotice, "Información completa del header WAV:");
				m_Logger.Write(FromKernel, LogNotice, "  Chunk ID: %s", header.chunkID);
				m_Logger.Write(FromKernel, LogNotice, "  Chunk Size: %u", header.chunkSize);
				m_Logger.Write(FromKernel, LogNotice, "  Format: %s", header.format);
				m_Logger.Write(FromKernel, LogNotice, "  Subchunk1 ID: %s", header.subChunk1ID);
				m_Logger.Write(FromKernel, LogNotice, "  Subchunk1 Size: %u", header.subChunk1Size);
				m_Logger.Write(FromKernel, LogNotice, "  Audio Format: %u", header.audioFormat);
				m_Logger.Write(FromKernel, LogNotice, "  Número de canales: %u", header.numChannels);
				m_Logger.Write(FromKernel, LogNotice, "  Frecuencia de muestreo: %u", header.sampleRate);
				m_Logger.Write(FromKernel, LogNotice, "  Byte Rate: %u", header.byteRate);
				m_Logger.Write(FromKernel, LogNotice, "  Block Align: %u", header.blockAlign);
				m_Logger.Write(FromKernel, LogNotice, "  Bits per Sample: %u", header.bitsPerSample);
				m_Logger.Write(FromKernel, LogNotice, "  Subchunk2 ID: %s", header.subChunk2ID);
				m_Logger.Write(FromKernel, LogNotice, "  Subchunk2 Size: %u", header.subChunk2Size);

				// Validar que el header tenga el formato esperado
				if (strncmp(header.chunkID, "RIFF", 4) != 0)
				{
					m_Logger.Write(FromKernel, LogError, "Error: Chunk ID no es 'RIFF'");
				}
				if (strncmp(header.format, "WAVE", 4) != 0)
				{
					m_Logger.Write(FromKernel, LogError, "Error: Format no es 'WAVE'");
				}
				if (header.subChunk1Size != 16)
				{
					m_Logger.Write(FromKernel, LogError, "Error: Subchunk1 Size es %u, se esperaba 16 para PCM", header.subChunk1Size);
				}
				if (header.audioFormat != 1)
				{
					m_Logger.Write(FromKernel, LogError, "Error: Audio Format es %u, se esperaba 1 para PCM", header.audioFormat);
				}	
				//     // Aquí podrías procesar los datos de audio...
			}
			// cierre del archivo
			// if (!m_FileSystem.FileClose (sample)) {
			// 	m_Logger.Write (FromKernel, LogPanic, "No se pudo cerrar el archivo");
			// }
		}
		// }

		// return ShutdownHalt;

		// APERTURA DE ARCHIVOS ------- FIN

		// configure sound device
		if (!m_pSound->AllocateQueue (QUEUE_SIZE_MSECS)) // Creación o asignación de tamaño de buffer de audio en MS (100)
		{
			m_Logger.Write (FromKernel, LogPanic, "No se pudo ubicar la cola de sonido");
		}

		m_pSound->SetWriteFormat (FORMAT, WRITE_CHANNELS); // debe determinar cómo va a enviar los paquetes de bytes (en fn a los canales y bit depth)

		// initially fill the whole queue with data
		unsigned nQueueSizeFrames = m_pSound->GetQueueSizeFrames (); // se obtiene el tamaño del buffer pero en frames

		unsigned remainingBytesToRead = header.subChunk2Size;

		writeWavData (nQueueSizeFrames, sample, remainingBytesToRead);

		// start sound device
		if (!m_pSound->Start ())
		{
			m_Logger.Write (FromKernel, LogPanic, "Cannot start sound device");
		}

		m_Logger.Write (FromKernel, LogNotice, "Se inicio el audio");

		int i = 0;
		while (m_pSound->IsActive() && remainingBytesToRead > 0) {

			writeWavData(nQueueSizeFrames - m_pSound->GetQueueFramesAvail(), sample, remainingBytesToRead);
			i++;
			m_Logger.Write (FromKernel, LogNotice, "Número de iteraciones secundarias %d", i);
		}

		PrintMemoryInfo();

		if (!m_FileSystem.FileClose (sample)) {
			m_Logger.Write (FromKernel, LogPanic, "No se pudo cerrar el archivo");
		}

		m_Scheduler.MsSleep (5000);

		m_Logger.Write (FromKernel, LogNotice, "FINALIZÓ LA EJECUCIÓN <3");

		// AQUI DEBE TERMINAR EL CICLO
	// }



	// m_Logger.Write (FromKernel, LogNotice, "Playing modulated 440 Hz tone");



	// // output sound data
	// for (unsigned nCount = 0; m_pSound->IsActive (); nCount++)
	// {
	// 	m_Scheduler.MsSleep (QUEUE_SIZE_MSECS / 2);

	// 	// fill the whole queue free space with data
	// 	WriteSoundData (nQueueSizeFrames - m_pSound->GetQueueFramesAvail ());

	// 	m_Screen.Rotor (0, nCount);
	// }

	// const char *mensaje = "--------- Hola desde Circle\r\n";
	// m_Serial.Write(mensaje, strlen(mensaje));
	// while (1) {
	// 	char c;
	// 	int nRead = m_Serial.Read(&c, 1);
	// 	if (nRead > 0) {
	// 		if (c == '1') {
	// 			m_ActLED.On();
	// 		}
	// 		else 
	// 			m_ActLED.Off();
	// 	}
	// }

	return ShutdownHalt;
}

void CKernel::writeWavData(unsigned nFrames, unsigned file, unsigned &remainingBytes) {
	// nFrames dice cuánto espacio (en frames) tengo en el buffer general de audio

	const unsigned nFramesPerWrite = 1024;

	u8 bufferToCircle[nFramesPerWrite * WRITE_CHANNELS * TYPE_SIZE];
	// el fragmento de datos que se mandará al audio gestionado por circle, es un buffer temporal.

	while (nFrames > 0 && remainingBytes > 0) {
		unsigned nWriteFrames = nFrames < nFramesPerWrite ? nFrames : nFramesPerWrite;
		// si el número de frames disponibles es menor que el número de frames por escritura,
		// se escribiá ese número menor, sino toma el tamaño máximo de frames que se pueden escribir. 

		// equivalencia de frames a bytes.
		unsigned nBytesByIter = nWriteFrames * WRITE_CHANNELS * TYPE_SIZE;
		
		// obtener datos de la microsd

		// saber el tamaño de frames que puedo leer de la microsd
		unsigned nReadedBytes = remainingBytes < nBytesByIter ? remainingBytes : nBytesByIter;
		
		unsigned int chunkWavFile = m_FileSystem.FileRead(file, bufferToCircle, nReadedBytes);
		if (chunkWavFile == FS_ERROR) {
			m_Logger.Write (FromKernel, LogPanic, "Error al leer fragmento");
		}

		// se terminaron de obtener los datos de la microsd

		int nResult = m_pSound->Write(bufferToCircle, nReadedBytes);
		if (nResult != (int)nReadedBytes) {
			m_Logger.Write(FromKernel, LogError, "no se pudo escribir el bloque") ;
		}
	
		nFrames -= nWriteFrames;
		remainingBytes -= nReadedBytes;
		m_Logger.Write(FromKernel, LogNotice, "Frames restantes en el buffer %d, bytes disponibles en la sd %d", nFrames, remainingBytes);
	}
}

void CKernel::WriteSoundData (unsigned nFrames)
{
	const unsigned nFramesPerWrite = 1000;
	u8 Buffer[nFramesPerWrite * WRITE_CHANNELS * TYPE_SIZE];

	while (nFrames > 0)
	{
		unsigned nWriteFrames = nFrames < nFramesPerWrite ? nFrames : nFramesPerWrite;
		// determina si la iteración actual es la última (al verificar si caben todos los frames contemplados
		// o si ya se manda el sobrante de frames)

		GetSoundData (Buffer, nWriteFrames);

		unsigned nWriteBytes = nWriteFrames * WRITE_CHANNELS * TYPE_SIZE;

		int nResult = m_pSound->Write (Buffer, nWriteBytes);
		if (nResult != (int) nWriteBytes)
		{
			m_Logger.Write (FromKernel, LogError, "Sound data dropped");
		}

		nFrames -= nWriteFrames;

		m_Scheduler.Yield ();		// ensure the VCHIQ tasks can run
	}
}

void CKernel::GetSoundData (void *pBuffer, unsigned nFrames)
{
	static float phase = 0.0f;  // fase acumulada
	float phaseInc = 2.0f * 3.1415926f * 440.0f / 48000;

    u8 *pBuffer8 = (u8 *) pBuffer;
    for (unsigned i = 0; i < nFrames * WRITE_CHANNELS; i++)
    {
        float sample = 0.9f * sinf(phase); // Valor entre -1 y +1
        phase += phaseInc;
        if (phase >= 2.0f * 3.1415926f) {
            phase -= 2.0f * 3.1415926f;
        }

        // Convertir a 16 bits:
        s16 nLevel = (s16)(sample * 32767);

        // Copiar a buffer...
        memcpy(&pBuffer8[i * sizeof(s16)], &nLevel, sizeof(s16));
    }
}

void CKernel::PrintMemoryInfo()
{
    CMemorySystem *pMemory = CMemorySystem::Get();
    if (pMemory)
    {
        size_t totalMemory = pMemory->GetMemSize();  // Memoria total del sistema
        size_t freeHeapMemory = pMemory->GetHeapFreeSpace(HEAP_ANY);  // Memoria libre en el heap
        size_t usedMemory = totalMemory - freeHeapMemory;  // Memoria utilizada

        // Conversión a MB
        float totalMemoryMB = totalMemory / (1024.0 * 1024.0);
        float freeMemoryMB = freeHeapMemory / (1024.0 * 1024.0);
        float usedMemoryMB = usedMemory / (1024.0 * 1024.0);

        // Cálculo de porcentajes
        float freeMemoryPercentage = (totalMemory > 0) ? ((freeHeapMemory * 100.0) / totalMemory) : 0;
        float usedMemoryPercentage = 100.0 - freeMemoryPercentage;

        // Log de la información
        m_Logger.Write(FromKernel, LogNotice, "Memoria Total: %u bytes (%.2f MB)", totalMemory, totalMemoryMB);
        m_Logger.Write(FromKernel, LogNotice, "Memoria Usada: %u bytes (%.2f MB) - %.2f%%", usedMemory, usedMemoryMB, usedMemoryPercentage);
        m_Logger.Write(FromKernel, LogNotice, "Memoria Libre: %u bytes (%.2f MB) - %.2f%%", freeHeapMemory, freeMemoryMB, freeMemoryPercentage);
    }
    else
    {
        m_Logger.Write(FromKernel, LogError, "Error: No se pudo obtener la información de memoria");
    }
}