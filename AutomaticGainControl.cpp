#include "AutomaticGainControl.h"
#include "math.h"

#define AGCMAX 12
#define AGCMIN -12

AutomaticGainControl::AutomaticGainControl()
    :m_agcAmpOut()
{
    m_agcHangtime = 100;
    m_agcSlopeIncrease = 0.05;
    m_agcSlopeDecrease = 0.5;
    m_agcThresh = 0.9;
}


double AutomaticGainControl::getAGC(double peak)
{
    m_peak = peak;
    static int m_hangtimer;
    static double m_agcGain = 0;
    static double m_agcGainOut;

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

