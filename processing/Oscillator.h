#ifndef __OSCILLATOR_H__
#define __OSCILLATOR_H__

#include "IPlug_include_in_plug_hdr.h"

#include "AudioProcessor.h"

#include <cmath>

#define M_PI 3.14159265358979323846

enum OscillatorParameters {
	kOscillatorFrequencyParam,
	kOscillatorModeParam,
	kNumOscillatorParams,
};

enum OscillatorMode {
	kModeSine,
	kModeTriangle,
	kModeSquare,
	kModeSawtooth,
	kNumOscillatorModes
};

class Oscillator : public AudioProcessor
{
public:
	Oscillator(IPlugBase* pPlug);
	~Oscillator();

	double GetNextSample();

	void SetMode(int newMode);
	void SetFrequency(double frequency);

protected:
	void HandleParamChange(int paramType, double newValue, int newIntValue) override;
	void HandleHostReset() override;

private:
	int mOscillatorMode;
	double mFrequency;

	double mPhasePosition;
	double mPhaseIncrement;

	void UpdatePhaseIncrement();
};

#endif // !__OSCILLATOR_H__
