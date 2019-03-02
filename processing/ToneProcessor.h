#ifndef __TONE_PROCESSOR_H__
#define __TONE_PROCESSOR_H__

class ToneProcessor
{
public:
	ToneProcessor();
	~ToneProcessor();

	double GetAdjustedSample(double input, double output);

	void SetMixPercent(double mixPercent);
	void SetMixFactor(double mixFactor);

private:
	double mMixFactor;
};

#endif // !__TONE_PROCESSOR_H__
