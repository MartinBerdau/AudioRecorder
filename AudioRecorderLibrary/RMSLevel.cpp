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
#include "RMSLevel.h"

RMSLevel::RMSLevel(double tau, double fs)
{
    m_tau = tau;
    m_fs = fs;
    calculateAlpha();
}


void RMSLevel::setTau(double tau)
{
    m_tau = tau;
    calculateAlpha();
}

void RMSLevel::setSampleFreq(double fs)
{
    m_fs = fs;
    calculateAlpha();
}

void RMSLevel::calculateAlpha()
{
    m_alpha = exp(-2/(m_tau * m_fs));
}

double RMSLevel::updateRMS(double curValue)
{
    m_data[1] = m_data[0];
    m_data[0] = (1-m_alpha)*curValue + m_alpha*m_data[1];
    return 20*log10(m_data[0]);
    /*
        y(k) - alpha*y(k-1) = (1-alpha)*x(k)
     -> y(k) = (1-alpha)*x(k) + alpha*y(k-1)
     */
}
