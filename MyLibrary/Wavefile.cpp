#include "Wavefile.h"

Wavefile::Wavefile()
{
}

Wavefile::~Wavefile()
{
}


void Wavefile::generateHeader(File& frec, char fileName, unsigned int recBytes)
{
    Subchunk2Size = recBytes;
    ChunkSize = Subchunk2Size + 36;   // Gesamte File-Länge 
    frec.write("RIFF");
    byte1 = ChunkSize & 0xff;
    byte2 = (ChunkSize >> 8) & 0xff;
    byte3 = (ChunkSize >> 16) & 0xff;
    byte4 = (ChunkSize >> 24) & 0xff;
    frec.write(byte1);  frec.write(byte2);  frec.write(byte3);  frec.write(byte4);
    frec.write("WAVE");
    // Ende des RIFF-Headers

    // Start des Format-Abschnitts  
    frec.write("fmt ");
    byte1 = Subchunk1Size & 0xff;
    byte2 = (Subchunk1Size >> 8) & 0xff;
    byte3 = (Subchunk1Size >> 16) & 0xff;
    byte4 = (Subchunk1Size >> 24) & 0xff;
    frec.write(byte1);  frec.write(byte2);  frec.write(byte3);  frec.write(byte4);
    byte1 = AudioFormat & 0xff;
    byte2 = (AudioFormat >> 8) & 0xff;
    frec.write(byte1);  frec.write(byte2);
    byte1 = numChannels & 0xff;
    byte2 = (numChannels >> 8) & 0xff;
    frec.write(byte1);  frec.write(byte2);
    byte1 = sampleRate & 0xff;
    byte2 = (sampleRate >> 8) & 0xff;
    byte3 = (sampleRate >> 16) & 0xff;
    byte4 = (sampleRate >> 24) & 0xff;
    frec.write(byte1);  frec.write(byte2);  frec.write(byte3);  frec.write(byte4);
    byte1 = byteRate & 0xff;
    byte2 = (byteRate >> 8) & 0xff;
    byte3 = (byteRate >> 16) & 0xff;
    byte4 = (byteRate >> 24) & 0xff;
    frec.write(byte1);  frec.write(byte2);  frec.write(byte3);  frec.write(byte4);
    byte1 = blockAlign & 0xff;
    byte2 = (blockAlign >> 8) & 0xff;
    frec.write(byte1);  frec.write(byte2);
    byte1 = bitsPerSample & 0xff;
    byte2 = (bitsPerSample >> 8) & 0xff;
    frec.write(byte1);  frec.write(byte2);

    frec.write("data");
    byte1 = Subchunk2Size & 0xff;
    byte2 = (Subchunk2Size >> 8) & 0xff;
    byte3 = (Subchunk2Size >> 16) & 0xff;
    byte4 = (Subchunk2Size >> 24) & 0xff;
    frec.write(byte1);  frec.write(byte2);  frec.write(byte3);  frec.write(byte4);
    frec.close();
    Serial.println("header written");
}
