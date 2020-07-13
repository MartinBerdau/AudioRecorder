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

//updated version

#pragma once

#include <cmath>

class AGC
{
public:

	enum timeConstants
	{
		fast = 0,
		medium,
		slow,
        veryslow
	};

	AGC();
    
    // Kompressorparameter
	void setSetpoint(double setpoint_dB) { m_setpoint_dB = setpoint_dB; };
    void setRange(double range_dB) {m_range_dB = range_dB; computeRangeAndRatio();};
    void setRatio(double ratio) {m_ratio = ratio; computeRangeAndRatio();};
    void setMaxGainRed(double maxGainRed) {m_maxGainRed = maxGainRed; computeRangeAndRatio();};
    void setMaxGainAmp(double maxGainAmp) {m_maxGainAmp = maxGainAmp; computeRangeAndRatio();};
    
    // Glättungsparameter
    void setTimeConst(AGC::timeConstants timeConst);
    void setRefreshRate(double refreshrate) { m_refreshRate = refreshrate; computeAlpha(); };
    
    // Gainberechnung
	double getAGCGain(double peak);

private:
	// Kompressorparameter einstellbar
    double m_setpoint_dB;
	double m_range_dB;
    double m_ratio;
    double m_maxGainRed;
    double m_maxGainAmp;
    
    // Kompressoparameter fest
    double m_maxThresh;
    
    // Kompressorparameter berechnet
    double m_upperThresh;
    double m_lowerThresh;
    double m_upperRatio;
	double m_lowerRatio;

    // Glättungsparameter
    double m_tau_s;
    double m_refreshRate;
    double m_alpha;
	double m_data[2];

    void computeRangeAndRatio();
	void computeAlpha();

};

