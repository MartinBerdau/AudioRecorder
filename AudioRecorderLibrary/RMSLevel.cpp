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
