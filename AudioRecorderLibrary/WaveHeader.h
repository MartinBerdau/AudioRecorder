/*
Copyright 2020 <Name>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR
A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

<kurze Erklärung, auf Englisch>

To Do
--> Links zu Quellen ggf.
--> nicht selbsterklärende Stellen im Code kommentieren
*/

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
