#pragma once
class AutomaticGainControl
{
public:
    AutomaticGainControl();
    double getAGC(double peak);
	
	void setAGCthresh(double AGCtresh)
	{m_agcThresh = AGCtresh;};
	
	void setAGCslopeInc(double AGCslopeIncrease)
	{m_agcSlopeIncrease = AGCslopeIncrease;};
	
	void setAGCslopeDec(double AGCslopeDecrease)
	{m_agcSlopeDecrease = AGCslopeDecrease;};
	
	void setAGChangtime(int AGChangtime)
	{m_agcHangtime = AGChangtime;};

protected:
    int m_agcHangtime;
    double m_agcThresh;
    double m_agcSlopeIncrease;
    double m_agcSlopeDecrease;
    double m_peak;
    double m_agcAmpOut;

};
