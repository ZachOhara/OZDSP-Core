#include "ToneProcessor.h"

ToneProcessor::ToneProcessor(IPlugBase* pPlug) :
	AudioProcessor(pPlug, kNumToneProcessorParams),
	mMixFactor(1.0)
{
}

ToneProcessor::~ToneProcessor()
{
}

double ToneProcessor::GetAdjustedSample(double input, double output)
{
	return (output * mMixFactor) + (input * (1.0 - mMixFactor));
}

void ToneProcessor::SetMixPercent(double mixPercent)
{
	SetMixFactor(mixPercent / 100.0);
}

void ToneProcessor::SetMixFactor(double mixFactor)
{
	mMixFactor = mixFactor;
}

void ToneProcessor::HandleParamChange(int paramType, double newValue, int newIntValue)
{
	switch (paramType)
	{
	case kToneProcessorMixPercentParam:
		SetMixPercent(newValue);
		break;
	default:
		break;
	}
}
