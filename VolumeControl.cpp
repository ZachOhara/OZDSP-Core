#include "VolumeControl.h"

VolumeControl::VolumeControl():
	mAmpFactor(1.0)
{
}

VolumeControl::~VolumeControl()
{
}

double VolumeControl::GetAdjustedSample(double sample)
{
	return sample * mAmpFactor;
}

void VolumeControl::SetDecibels(double decibels)
{
	SetAmpFactor(GetAmpFactor(decibels));
}

void VolumeControl::SetZero()
{
	SetAmpFactor(0.0);
}

void VolumeControl::SetAmpFactor(double ampFactor)
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
