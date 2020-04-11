#include <iostream>
#define _USE_MATH_DEFINES
#include <cmath>
#include "RMSLevel.h"

int main() {
    
    double tau = 0.125;
    double fs = 44100;
    int samplesTested = 2*floor(tau*fs);
    
    RMSLevel MessObj(tau,fs);
    
    for (unsigned int kk=0; kk<samplesTested; kk++) {
        std::cout << MessObj.updateRMS(0.707) << "\n";
    }
    
    /*double sampleRate(48000), frequency(sampleRate/4);
    
    //Aufruf Konstruktor
    SinusGenerator sinGen(1,0,sampleRate,frequency);
    
    //Datenspeicher aufbauen
    std::vector<double> sinData;
    int nrOfSamples = 8;
    sinData.resize(nrOfSamples);
    
    sinGen.setAmplitude(2);
    sinGen.setFrequency(100);
    
    //Methode getData zum genereieren der Daten nutzen
    sinGen.getData(sinData);
    
    for (auto kk=0; kk < sinData.size(); kk++)
    {
        std::cout << sinData.at(kk) << "\n";
    }*/
    return 0;
}
