#pragma once
#include <Arduino.h>
#include <SD.h>

class WaveHeader
{
public:
    WaveHeader();
    void writeWaveHeader(unsigned long recByteSaved, File frec);

private:
    // Variablen für den Wave-Header
    unsigned long m_ChunkSize;
    unsigned long m_Subchunk1Size;
    unsigned int m_AudioFormat;
    unsigned int m_numChannels;
    unsigned long m_sampleRate;
    unsigned int m_bitsPerSample;
    unsigned long m_byteRate;
    unsigned int m_blockAlign;
    unsigned long m_Subchunk2Size;
    unsigned long m_recByteSaved;
    unsigned long m_NumSamples;
    byte byte1, byte2, byte3, byte4;
    File m_frec;
};