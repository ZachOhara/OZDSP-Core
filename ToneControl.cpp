#include "ToneControl.h"

ToneControl::ToneControl() :
	mMixFactor(1.0)
{
}

ToneControl::~ToneControl()
{
}

double ToneControl::GetAdjustedSample(double input, double output)
{
	return (output * mMixFactor) + (input * (1.0 - mMixFactor));
}

void ToneControl::SetMixPercent(double mixPercent)
{
	SetMixFactor(mixPercent / 100.0);
}

void ToneControl::SetMixFactor(double mixFactor)
{
	mMixFactor = mixFactor;
}
