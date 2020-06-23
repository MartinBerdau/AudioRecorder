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