#include "VolumeProcessor.h"

VolumeProcessor::VolumeProcessor():
	ModularProcessor(kNumParams)
{
	SetAmpFactor(1.0);
}

VolumeProcessor::~VolumeProcessor()
{
}

double VolumeProcessor::GetAdjustedSample(double sample)
{
	return sample * mAmpFactor;
}

void VolumeProcessor::SetDecibels(double decibels)
{
	SetAmpFactor(DecibelsToAmpFactor(decibels));
}

void VolumeProcessor::SetLoudness(double loudness)
{
	SetAmpFactor(LoudnessToAmpFactor(loudness));
}

void VolumeProcessor::SetZero()
{
	SetAmpFactor(0.0);
}

void VolumeProcessor::SetAmpFactor(double ampFactor)
{
	mAmpFactor = ampFactor;
}

void VolumeProcessor::HandleParamChange(int paramType, double newValue, int newIntValue)
{
	switch (paramType) {
	case kDecibelsParam:
		GetDecibelsFromParam(newValue);
		break;
	case kLoudnessParam:
		SetLoudness(newValue);
		break;
	}
}

void VolumeProcessor::GetDecibelsFromParam(double paramValue)
{
	static const double threshold_db = -99.95;
	IParam* pParam = GetParamObject(kDecibelsParam);
	double minValue = pParam->GetMin();

	// If the value is below the threshold, just set it to the minimum
	if (paramValue <= threshold_db && paramValue != minValue)
		pParam->Set(minValue);

	// Now update the processor
	if (paramValue != minValue)
		SetDecibels(paramValue);
	else
		SetZero();
}

double DecibelsToAmpFactor(double decibels)
{
	return sqrt(pow(10, decibels / 10));
}

double AmpFactorToDecibels(double ampFactor)
{
	return 10 * log10(pow(ampFactor, 2));
}

double LoudnessToAmpFactor(double loudness)
{
	return pow(loudness, 3.0 / 2.0);
}
