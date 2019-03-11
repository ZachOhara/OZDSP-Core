#ifndef __TONE_PROCESSOR_H__
#define __TONE_PROCESSOR_H__

#include "IPlug_include_in_plug_hdr.h"

#include "AudioProcessor.h"

enum ToneProcessorParameters
{
	kToneProcessorMixPercentParam,
	kNumToneProcessorParams
};

class ToneProcessor : public AudioProcessor
{
public:
	ToneProcessor(IPlugBase* pPlug);
	~ToneProcessor();

	double GetAdjustedSample(double input, double output);

	void SetMixPercent(double mixPercent);
	void SetMixFactor(double mixFactor);

protected:
	void HandleParamChange(int paramType, double newValue, int newIntValue) override;

private:
	double mMixFactor;
};

#endif // !__TONE_PROCESSOR_H__
