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
#include <math.h>

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
