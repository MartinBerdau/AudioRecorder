#pragma once
#include "Arduino.h"
#include "SD.h"

class Wavefile
{
public:
	Wavefile();
	~Wavefile();
	void generateHeader(File& frec, char fileName, unsigned int recBytes);

private:
	File& frec;
	char m_fileName;
	unsigned long m_recBytes;

	unsigned long ChunkSize = 0L;
	unsigned long Subchunk1Size = 16;
	unsigned int AudioFormat = 1;         // PCM = unkomprimiert
	unsigned int numChannels = 1;         // 1 = mono, 2 = Stereo
	unsigned long sampleRate = 44100;
	unsigned int bitsPerSample = 16;
	unsigned long byteRate = sampleRate * numChannels * (bitsPerSample / 8);// samplerate x channels x (bitspersample / 8)
	unsigned int blockAlign = numChannels * bitsPerSample / 8;
	unsigned long Subchunk2Size = 0L;
	unsigned long recByteSaved = 0L;
	unsigned long NumSamples = 0L;
	byte byte1, byte2, byte3, byte4;
};

