#include "AutomaticGainControl.h"
#include "math.h"

#define AGCMAX 12
#define AGCMIN -12

AutomaticGainControl::AutomaticGainControl()
{
    m_agcAmpOut = 0;
    m_hangtimer = 0;
    m_agcGainOut = 0;
    m_agcGain = 0;
    m_agcHangtime = 100;
    m_agcSlopeIncrease = 0.05;
    m_agcSlopeDecrease = 0.5;
    m_agcThresh = 0.9;
}

double AutomaticGainControl::getAGC(double peak)
{

    m_peak = peak;
    // Gain Decrease
    if (m_peak > m_agcThresh)
    {
        m_agcGainOut = m_agcGain - m_agcSlopeDecrease;

        // Reduce limit 
        if (m_agcGainOut <= AGCMIN) m_agcGainOut = AGCMIN;

        m_agcGain = m_agcGainOut;

        // Reset hangtimer
        m_hangtimer = 0;
    }

    // hangtimer before increasing
    else if (m_peak < m_agcThresh)
    {
        m_hangtimer++;
    }

    // Gain Increase
    if (m_hangtimer >= m_agcHangtime)
    {
        m_agcGainOut = m_agcGain + m_agcSlopeIncrease;

        // Gain limit
        if (m_agcGainOut >= AGCMAX) m_agcGainOut = AGCMAX;

        m_agcGain = m_agcGainOut;
    }
    m_agcAmpOut = pow(10, m_agcGainOut / 20);
    return m_agcAmpOut;
}

void AutomaticGainControl::setAGChangtime(int hangSetting)
{
    m_hangSetting = hangSetting;
    if (m_hangSetting == 1)
        m_agcHangtime = 0;

    if (m_hangSetting == 2)
        m_agcHangtime = 50;

    if (m_hangSetting == 3)
        m_agcHangtime = 100;

    if (m_hangSetting == 4)
        m_agcHangtime = 150;
}


void AutomaticGainControl::setAGCslopeInc(double slopeIncSetting)
{
    m_slopeIncSetting = slopeIncSetting;
    if (m_slopeIncSetting == 1)
        m_agcSlopeIncrease = 0.02;

    if (m_slopeIncSetting == 2)
        m_agcSlopeIncrease = 0.05;

    if (m_slopeIncSetting == 3)
        m_agcSlopeIncrease = 0.1;

    if (m_slopeIncSetting == 4)
        m_agcSlopeIncrease = 0.25;
}

void AutomaticGainControl::setAGCslopeDec(double slopeDecSetting)
{
    m_slopeDecSetting = slopeDecSetting;
    if (m_slopeDecSetting == 1)
        m_agcSlopeDecrease = 0.2;

    if (m_slopeDecSetting == 2)
        m_agcSlopeDecrease = 0.5;

    if (m_slopeDecSetting == 3)
        m_agcSlopeDecrease = 1.0;

    if (m_slopeDecSetting == 4)
        m_agcSlopeDecrease = 1.5;
}


