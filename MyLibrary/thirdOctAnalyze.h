/* Header kommt noch

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
