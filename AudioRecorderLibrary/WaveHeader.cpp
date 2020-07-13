/*
Copyright 2020 Jarvus Chen, changes by Timm-Jonas Bäumer

This file is licensed under GitHubs TOS.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR
A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Creates the Header for a WAV-File by writing the following sections to the
beginning of the recorded file:
    1. RIFF-Header: contains 'RIFF', the file-length and 'WAVE'
    2. Format-Header: contains additional data such as the samplerate and number 
       of channels
    3. Data-Section: contains 'data', the recording-length and the recording itself
    
Source: This file is based on code by Jarvus Chen: 
 https://gist.github.com/JarvusChen/fb641cad18eca4988a9e83a9ce65f42f
*/

#include "WaveHeader.h"

WaveHeader::WaveHeader()
    :m_ChunkSize(0L),
    m_Subchunk1Size(16),
    m_AudioFormat(1),         // PCM = uncompressed
    m_numChannels(1),         // 1 = mono, 2 = stereo
    m_sampleRate(44100),
    m_bitsPerSample(16),      
    m_Subchunk2Size(0L),
    m_recByteSaved(0L),
    m_NumSamples(0L)

{
  m_byteRate = m_sampleRate * m_numChannels * (m_bitsPerSample / 8);
  m_blockAlign = m_numChannels * m_bitsPerSample / 8;   
}

void WaveHeader::writeWaveHeader(unsigned long recByteSaved, File frec)
{
    m_recByteSaved = recByteSaved;        // amount of bytes of the current recording
    m_frec = frec;                        // the current recording-file

    // start of RIFF-header
    m_Subchunk2Size = m_recByteSaved;      
    m_ChunkSize = m_Subchunk2Size + 36;   // length of recording + header -8
    m_frec.seek(0);                       // finds the start of the recording
    m_frec.write("RIFF");
    byte1 = m_ChunkSize & 0xff;
    byte2 = (m_ChunkSize >> 8) & 0xff;
    byte3 = (m_ChunkSize >> 16) & 0xff;
    byte4 = (m_ChunkSize >> 24) & 0xff;
    m_frec.write(byte1);  m_frec.write(byte2);  m_frec.write(byte3);  m_frec.write(byte4);
    m_frec.write("WAVE");
    // end of RIFF-header

    // start of format-header
    m_frec.write("fmt ");
    byte1 = m_Subchunk1Size & 0xff;       // length of format header
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
    // end of format-header

    // start of data-section
    m_frec.write("data");
    byte1 = m_Subchunk2Size & 0xff;
    byte2 = (m_Subchunk2Size >> 8) & 0xff;
    byte3 = (m_Subchunk2Size >> 16) & 0xff;
    byte4 = (m_Subchunk2Size >> 24) & 0xff;
    m_frec.write(byte1);  m_frec.write(byte2);  m_frec.write(byte3);  m_frec.write(byte4);
    m_frec.close();
    Serial.println("header written");
}
