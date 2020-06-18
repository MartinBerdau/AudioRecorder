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

