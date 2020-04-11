/* Header kommt noch

Idee:   analyze_rms nutzen, um einen RMS-Wert als Faktor zu erhalten
        und dann mit dieser Klasse einen geglätteten RMS-Wert in dbFS
        berechnen. Dazu Glättungszeit Tau in Sekunden angeben
        (typisch: slow->1s, fast->0.125s).
Prinzip:Glättung mit TP-Filter 1. Ordnung mit folgenden Koeffizienten:
        b = [1-alpha], a = [1, -alpha],
        mit alpha = exp(-2/(Tau * SampleFrequenz))
 
        in Differenzen-Gleichung-Form:
            a1 * y(k) + a2 * y(k-1)  =  b1 * x(k)
        ->  y(k) - alpha * y(k-1) = (1-alpha) * x(k)
        ->  y(k) = (1-alpha) * x(k) + alpha * y(k-1)
 */

#pragma once
#import <math.h>

class RMSLevel{
    
public:
    RMSLevel(double tau, double fs);
    void setTau(double tau);
    void setSampleFreq(double fs);
    void calculateAlpha();
    double updateRMS(double curValue);
    
protected:
    double m_tau;
    double m_fs;
    
    double m_data[2];
    
    double m_alpha;
};
