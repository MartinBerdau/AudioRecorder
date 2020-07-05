//Updated version

#include "AGC.h"

AGC::AGC()
{
	// Kompressorparameter
    m_maxThresh = -1;
    
    m_setpoint_dB = -14.0;
    m_range_dB = 6.0;
    m_ratio = 3.0;
    m_maxGainRed = -12.0;
    m_maxGainAmp = 12.0;
    computeRangeAndRatio();
    
    // Glättungsparameter
    m_refreshRate = 100.0;
    setTimeConst(AGC::timeConstants::medium);
    computeAlpha();

	m_data[0] = 1.0;

}

void AGC::setTimeConst(AGC::timeConstants timeConst)
{
	switch (timeConst)
	{
	case AGC::timeConstants::fast:
		m_tau_s = 1.0 / 8.0;
		break;
	case AGC::timeConstants::medium:
		m_tau_s = 1.0 / 4.0;
		break;
	case AGC::timeConstants::slow:
		m_tau_s = 1.0;
		break;
	case AGC::timeConstants::veryslow:
		m_tau_s = 1.5;
		break;
	}
	computeAlpha();
}

double AGC::getAGCGain(double peak)
{
	double x_dB = 20.0 * log10(peak);
	m_data[1] = m_data[0];
	m_data[0] = (1.0 - m_alpha) * fmin((1.0 / m_upperRatio - 1.0) * (x_dB - m_upperThresh) , fmax( (1.0/m_lowerRatio-1)*(x_dB-m_lowerThresh) , 0)) +
		m_alpha * m_data[1];
	return pow(10.0,
               fmax(m_maxGainRed,fmin(m_data[0],m_maxGainAmp))
               /20.0);
}

void AGC::computeRangeAndRatio()
{
    m_lowerThresh = m_setpoint_dB - 0.5 * m_range_dB;
    m_upperThresh = fmin(m_setpoint_dB + 0.5 * m_range_dB , m_maxThresh);
    
    m_lowerRatio = m_ratio;
    m_upperRatio = fmax(-m_maxGainRed/-m_upperThresh,m_ratio);
}

void AGC::computeAlpha()
{
	m_alpha = exp(-2.0 / (m_tau_s * m_refreshRate));
}
