#ifndef __TONE_MIX_PROCESSOR_H__
#define __TONE_MIX_PROCESSOR_H__

#include "IPlug_include_in_plug_hdr.h"

#include "ModularProcessor.h"

class ToneMixProcessor : public ModularProcessor
{
public:
	enum EParameters {kMixPercentParam, kNumParams};

	ToneMixProcessor(IPlugBase* pPlug);
	~ToneMixProcessor();

	double GetAdjustedSample(double input, double output);

	void SetMixPercent(double mixPercent);
	void SetMixFactor(double mixFactor);

protected:
	void HandleParamChange(int paramType, double newValue, int newIntValue) override;

private:
	double mMixFactor;
};

#endif // !__TONE_PROCESSOR_H__
