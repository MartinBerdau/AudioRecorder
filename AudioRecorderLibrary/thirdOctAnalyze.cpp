/*
Copyright 2020 Martin Berdau

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

This file computes a third octave spectrum from a fft from 100 to 4000 Hz (when
using a sampling frequency of 44100 Hz) by adding fft bins according to third
octave filter bandwidth. 
 
 Source:
 logarithmische Aufteilung der Bänder:
 https://forum.pjrc.com/threads/32677-Is-there-a-logarithmic-function-for-FFT-bin-selection-for-any-given-of-bands?p=142524&viewfull=1#post142524
 3rd Oct:
 https://forum.pjrc.com/threads/32677-Is-there-a-logarithmic-function-for-FFT-bin-selection-for-any-given-of-bands?p=94941&viewfull=1#post94941
 */

#include "thirdOctAnalyze.h"

thirdOctAnalyze::thirdOctAnalyze(int averages)
{
    setConstants();
    m_averages = averages;
}

 void thirdOctAnalyze::setAverages(int averages){
    m_averages = averages;
}

void thirdOctAnalyze::updateData(AudioAnalyzeFFT1024 fft1024, double *dataVec){
    int curBin = m_startBin;
    
    for (int kk=0; kk < m_bands; kk++)
    {
        dataVec[kk] *= (m_averages - 1)/m_averages;
        dataVec[kk] += 1/m_averages * m_calibration[kk] *
        fft1024.read(curBin - m_lowerOverlap[kk],
        curBin + m_binsPerBand[kk] + m_upperOverlap[kk] - 1);
        curBin += m_binsPerBand[kk];
    }
}

void thirdOctAnalyze::reset(double *dataVec)
{
    for (int kk = 0; kk < m_bands; kk++)
    {
        dataVec[kk] = 0;
    }
}


void thirdOctAnalyze::setConstants()
{
    // 125 Hz
    m_binsPerBand[0] = 1;
    m_calibration[0] = 0.78;
    m_upperOverlap[0] = 1;
    m_lowerOverlap[0] = 1;
    // 160 Hz
    m_binsPerBand[1] = 1;
    m_calibration[1] = 0.98;
    m_upperOverlap[1] = 1;
    m_lowerOverlap[1] = 1;
    // 200 Hz
    m_binsPerBand[2] = 1;
    m_calibration[2] = 1.15;
    m_upperOverlap[2] = 1;
    m_lowerOverlap[2] = 1;
    // 250 Hz
    m_binsPerBand[3] = 2;
    m_calibration[3] = 0.98;
    m_upperOverlap[3] = 1;
    m_lowerOverlap[3] = 1;
    // 315 Hz
    m_binsPerBand[4] = 1;
    m_calibration[4] = 1.47;
    m_upperOverlap[4] = 1;
    m_lowerOverlap[4] = 1;
    // 400 Hz
    m_binsPerBand[5] = 2;
    m_calibration[5] = 1.22;
    m_upperOverlap[5] = 1;
    m_lowerOverlap[5] = 1;
    // 500 Hz
    m_binsPerBand[6] = 3;
    m_calibration[6] = 1.09;
    m_upperOverlap[6] = 1;
    m_lowerOverlap[6] = 1;
    // 630 Hz
    m_binsPerBand[7] = 3;
    m_calibration[7] = 1.25;
    m_upperOverlap[7] = 1;
    m_lowerOverlap[7] = 1;
    // 800 Hz
    m_binsPerBand[8] = 5;
    m_calibration[8] = 1.02;
    m_upperOverlap[8] = 1;
    m_lowerOverlap[8] = 1;
    // 1 kHz
    m_binsPerBand[9] = 5;
    m_calibration[9] = 1.17;
    m_upperOverlap[9] = 1;
    m_lowerOverlap[9] = 1;
    // 1,25 kHz
    m_binsPerBand[10] = 7;
    m_calibration[10] = 1.02;
    m_upperOverlap[10] = 1;
    m_lowerOverlap[10] = 1;
    // 1,6 kHz
    m_binsPerBand[11] = 8;
    m_calibration[11] = 1.02;
    m_upperOverlap[11] = 1;
    m_lowerOverlap[11] = 1;
    // 2 kHz
    m_binsPerBand[12] = 11;
    m_calibration[12] = 0.9;
    m_upperOverlap[12] = 1;
    m_lowerOverlap[12] = 1;
    // 2,5 kHz
    m_binsPerBand[13] = 14;
    m_calibration[13] = 0.79;
    m_upperOverlap[13] = 1;
    m_lowerOverlap[13] = 1;
    // 3,15 kHz
    m_binsPerBand[14] = 17;
    m_calibration[14] = 0.76;
    m_upperOverlap[14] = 1;
    m_lowerOverlap[14] = 1;
    // 4 kHz
    m_binsPerBand[15] = 21;
    m_calibration[15] = 0.71;
    m_upperOverlap[15] = 1;
    m_lowerOverlap[15] = 1;
}
