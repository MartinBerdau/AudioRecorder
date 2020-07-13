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

#pragma once
#include <Arduino.h>
#include <Audio.h>

class thirdOctAnalyze{
    
public:
    thirdOctAnalyze(int averages);
    void setAverages(int averages);
    void updateData(AudioAnalyzeFFT1024 fft1024, double *dataVec);
    void reset(double *dataVec);
    
private:
    static const int m_bands = 16;
    double m_binsPerBand[m_bands];
    double m_calibration[m_bands];
    double m_upperOverlap[m_bands];
    double m_lowerOverlap[m_bands];
    void setConstants();
    
    static const int m_startBin = 2;
    double m_averages;
};
