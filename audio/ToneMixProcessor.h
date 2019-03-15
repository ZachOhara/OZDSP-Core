#ifndef __TONE_MIX_PROCESSOR_H__
#define __TONE_MIX_PROCESSOR_H__

#include "../ModularProcessor.h"

class ToneMixProcessor : public ModularProcessor
{
public:
	enum EParameters {kMixPercentParam, kNumParams};

	ToneMixProcessor();
	~ToneMixProcessor();

	double GetAdjustedSample(double input, double output);

	void SetMixPercent(double mixPercent);
	void SetMixFactor(double mixFactor);

protected:
	void HandleParamChange(int paramType, double newValue, int newIntValue) override;

private:
	double mMixFactor;
};

#endif // !__TONE_MIX_PROCESSOR_H__
