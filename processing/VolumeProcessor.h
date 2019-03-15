#ifndef __VOLUME_PROCESSOR_H__
#define __VOLUME_PROCESSOR_H__

#include "IPlug_include_in_plug_hdr.h"

#include "ModularProcessor.h"

#include <math.h>

double DecibelsToAmpFactor(double decibels);
double AmpFactorToDecibels(double ampFactor);
double LoudnessToAmpFactor(double loudness);

class VolumeProcessor : public ModularProcessor
{
public:
	enum EParameters {kDecibelsParam, kLoudnessParam, kNumParams};

	VolumeProcessor();
	~VolumeProcessor();

	double GetAdjustedSample(double sample);

	void SetDecibels(double decibels);
	void SetLoudness(double loudness);
	void SetZero();
	void SetAmpFactor(double ampFactor);

protected:
	void HandleParamChange(int paramType, double newValue, int newIntValue) override;

private:
	double mAmpFactor;

	void GetDecibelsFromParam(double paramValue);
};

#endif __VOLUME_PROCESSOR_H__

