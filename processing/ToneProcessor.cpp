#include "ToneProcessor.h"

ToneProcessor::ToneProcessor() :
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
