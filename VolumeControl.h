#ifndef __VOLUMECONTROL__
#define __VOLUMECONTROL__

#include <math.h>

double GetAmpFactor(double decibels);
double GetDecibels(double ampFactor);

class VolumeControl
{
public:
	VolumeControl();
	~VolumeControl();

	double GetAdjustedSample(double sample);

	void SetDecibels(double decibels);
	void SetZero();
	void SetAmpFactor(double ampFactor);

private:
	double mAmpFactor;
};

#endif __VOLUMECONTROL__

