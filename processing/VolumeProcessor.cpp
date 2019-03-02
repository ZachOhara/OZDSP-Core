#include "VolumeProcessor.h"

VolumeProcessor::VolumeProcessor():
	mAmpFactor(1.0)
{
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
	SetAmpFactor(GetAmpFactor(decibels));
}

void VolumeProcessor::SetZero()
{
	SetAmpFactor(0.0);
}

void VolumeProcessor::SetAmpFactor(double ampFactor)
{
	mAmpFactor = ampFactor;
}

double GetAmpFactor(double decibels)
{
	return sqrt(pow(10, decibels / 10));
}

double GetDecibels(double ampFactor)
{
	return 10 * log10(pow(ampFactor, 2));
}
