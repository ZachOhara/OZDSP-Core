#include "ToneMixProcessor.h"

ToneMixProcessor::ToneMixProcessor(IPlugBase* pPlug) :
	AudioProcessor(pPlug, kNumParams),
	mMixFactor(1.0)
{
}

ToneMixProcessor::~ToneMixProcessor()
{
}

double ToneMixProcessor::GetAdjustedSample(double input, double output)
{
	return (output * mMixFactor) + (input * (1.0 - mMixFactor));
}

void ToneMixProcessor::SetMixPercent(double mixPercent)
{
	SetMixFactor(mixPercent / 100.0);
}

void ToneMixProcessor::SetMixFactor(double mixFactor)
{
	mMixFactor = mixFactor;
}

void ToneMixProcessor::HandleParamChange(int paramType, double newValue, int newIntValue)
{
	switch (paramType)
	{
	case kMixPercentParam:
		SetMixPercent(newValue);
		break;
	}
}
