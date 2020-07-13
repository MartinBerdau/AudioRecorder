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

#pragma once
class AutomaticGainControl
{
public:
	AutomaticGainControl();
	double getAGC(double peak);
	void setAGCthresh(double AGCtresh) { m_agcThresh = AGCtresh; };
	void setAGCslopeInc(double slopeIncSetting);
	void setAGCslopeDec(double slopeDecSetting);
	void setAGChangtime(int hangSetting);

protected:
	int m_agcHangtime;
	double m_agcThresh;
	double m_agcSlopeIncrease;
	double m_agcSlopeDecrease;
	double m_peak;
	double m_agcAmpOut;
	int m_slopeIncSetting;
	int m_slopeDecSetting;
	int m_hangSetting;
	int m_hangtimer;
	double m_agcGainOut;
	double m_agcGain;
};
