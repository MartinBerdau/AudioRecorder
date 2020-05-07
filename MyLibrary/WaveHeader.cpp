#include "WaveHeader.h"

WaveHeader::WaveHeader()
    :m_ChunkSize(0L),
    m_Subchunk1Size(16),
    m_AudioFormat(1),         // PCM = unkomprimiert
    m_numChannels(1),         // 1 = mono, 2 = Stereo
    m_sampleRate(44100),
    m_bitsPerSample(16),
    m_Subchunk2Size(0L),
    m_recByteSaved(0L),
    m_NumSamples(0L)

{
  m_byteRate = m_sampleRate * m_numChannels * (m_bitsPerSample / 8);// samplerate x channels x (bitspersample / 8)
  m_blockAlign = m_numChannels * m_bitsPerSample / 8;
}

void WaveHeader::writeWaveHeader(unsigned long recByteSaved, File frec)
{
    // Quelle: https://gist.github.com/JarvusChen/fb641cad18eca4988a9e83a9ce65f42f

  //  m_NumSamples = (m_recByteSaved*8)/m_bitsPerSample/m_numChannels;
  //  m_Subchunk2Size = m_NumSamples*m_numChannels*m_bitsPerSample/8; // number of samples x number of channels x number of bytes per sample

    m_recByteSaved = recByteSaved;
    m_frec = frec;

    m_Subchunk2Size = m_recByteSaved;
    m_ChunkSize = m_Subchunk2Size + 36;   // Gesamte File-Länge 
    m_frec.seek(0);
    m_frec.write("RIFF");
    byte1 = m_ChunkSize & 0xff;
    byte2 = (m_ChunkSize >> 8) & 0xff;
    byte3 = (m_ChunkSize >> 16) & 0xff;
    byte4 = (m_ChunkSize >> 24) & 0xff;
    m_frec.write(byte1);  m_frec.write(byte2);  m_frec.write(byte3);  m_frec.write(byte4);
    m_frec.write("WAVE");
    // Ende des RIFF-Headers

    // Start des Format-Abschnitts  
    m_frec.write("fmt ");
    byte1 = m_Subchunk1Size & 0xff;
    byte2 = (m_Subchunk1Size >> 8) & 0xff;
    byte3 = (m_Subchunk1Size >> 16) & 0xff;
    byte4 = (m_Subchunk1Size >> 24) & 0xff;
    m_frec.write(byte1);  m_frec.write(byte2);  m_frec.write(byte3);  m_frec.write(byte4);
    byte1 = m_AudioFormat & 0xff;
    byte2 = (m_AudioFormat >> 8) & 0xff;
    m_frec.write(byte1);  m_frec.write(byte2);
    byte1 = m_numChannels & 0xff;
    byte2 = (m_numChannels >> 8) & 0xff;
    m_frec.write(byte1);  m_frec.write(byte2);
    byte1 = m_sampleRate & 0xff;
    byte2 = (m_sampleRate >> 8) & 0xff;
    byte3 = (m_sampleRate >> 16) & 0xff;
    byte4 = (m_sampleRate >> 24) & 0xff;
    m_frec.write(byte1);  m_frec.write(byte2);  m_frec.write(byte3);  m_frec.write(byte4);
    byte1 = m_byteRate & 0xff;
    byte2 = (m_byteRate >> 8) & 0xff;
    byte3 = (m_byteRate >> 16) & 0xff;
    byte4 = (m_byteRate >> 24) & 0xff;
    m_frec.write(byte1);  m_frec.write(byte2);  m_frec.write(byte3);  m_frec.write(byte4);
    byte1 = m_blockAlign & 0xff;
    byte2 = (m_blockAlign >> 8) & 0xff;
    m_frec.write(byte1);  m_frec.write(byte2);
    byte1 = m_bitsPerSample & 0xff;
    byte2 = (m_bitsPerSample >> 8) & 0xff;
    m_frec.write(byte1);  m_frec.write(byte2);
    // Ende des Format-Abschnitts

    // Anfang des Daten-Abschnitts
    m_frec.write("data");
    byte1 = m_Subchunk2Size & 0xff;
    byte2 = (m_Subchunk2Size >> 8) & 0xff;
    byte3 = (m_Subchunk2Size >> 16) & 0xff;
    byte4 = (m_Subchunk2Size >> 24) & 0xff;
    m_frec.write(byte1);  m_frec.write(byte2);  m_frec.write(byte3);  m_frec.write(byte4);
    m_frec.close();
    Serial.println("header written");
    //Serial.print("Subchunk2: "); 
    //Serial.println(m_Subchunk2Size); 
}
