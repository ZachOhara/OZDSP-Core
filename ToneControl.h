#ifndef __TONECONTROL__
#define __TONECONTROL__

class ToneControl
{
public:
	ToneControl();
	~ToneControl();

	double GetAdjustedSample(double input, double output);

	void SetMixPercent(double mixPercent);
	void SetMixFactor(double mixFactor);

private:
	double mMixFactor;
};

#endif // !__TONECONTROL__
