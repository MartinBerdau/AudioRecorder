/*
Copyright 2020 Hannes Sauerbaum
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



this function is used to automatically adjust the gain with the following parameters:
treshhold, slope increase, slope decrease and the time before increasing gain/hangtime
*/

#include "AutomaticGainControl.h"
#include "math.h"

#define AGCMAX 12                                       //max Value
#define AGCMIN -12                                      //min Value

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
    // gain decrease
    if (m_peak > m_agcThresh)
    {
        m_agcGainOut = m_agcGain - m_agcSlopeDecrease;
        // limit reduce  
        if (m_agcGainOut <= AGCMIN) m_agcGainOut = AGCMIN;
        m_agcGain = m_agcGainOut;

        // reset hangtimer
        m_hangtimer = 0;
    }

    // time before increasing
    else if (m_peak < m_agcThresh)
    {
        m_hangtimer++;
    }

    // gain increase
    if (m_hangtimer >= m_agcHangtime)
    {
        m_agcGainOut = m_agcGain + m_agcSlopeIncrease;

        // limit gain 
        if (m_agcGainOut >= AGCMAX) m_agcGainOut = AGCMAX;
        m_agcGain = m_agcGainOut;
    }
    //convert to linear scale
    m_agcAmpOut = pow(10, m_agcGainOut / 20);
    return m_agcAmpOut;
}

// setter functions
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
