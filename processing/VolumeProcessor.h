#ifndef __VOLUME_PROCESSOR_H__
#define __VOLUME_PROCESSOR_H__

#include "IPlug_include_in_plug_hdr.h"

#include "AudioProcessor.h"

#include <math.h>

double GetAmpFactor(double decibels);
double GetDecibels(double ampFactor);

enum VolumeProcessorParameters
{
	kVolumeProcessorDecibelsParam,
	kNumVolumeProcessorParams,
};

class VolumeProcessor : public AudioProcessor
{
public:
	VolumeProcessor(IPlugBase* pPlug);
	~VolumeProcessor();

	double GetAdjustedSample(double sample);

	void SetDecibels(double decibels);
	void SetZero();
	void SetAmpFactor(double ampFactor);

protected:
	void HandleParamChange(int paramType, double newValue, int newIntValue) override;

private:
	double mAmpFactor;

	void GetDecibelsFromParam(double paramValue);
};

#endif __VOLUME_PROCESSOR_H__

