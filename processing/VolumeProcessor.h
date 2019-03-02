#ifndef __VOLUME_PROCESSOR_H__
#define __VOLUME_PROCESSOR_H__

#include <math.h>

double GetAmpFactor(double decibels);
double GetDecibels(double ampFactor);

class VolumeProcessor
{
public:
	VolumeProcessor();
	~VolumeProcessor();

	double GetAdjustedSample(double sample);

	void SetDecibels(double decibels);
	void SetZero();
	void SetAmpFactor(double ampFactor);

private:
	double mAmpFactor;
};

#endif __VOLUME_PROCESSOR_H__

