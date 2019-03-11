#ifndef __OSCILLATOR_H__
#define __OSCILLATOR_H__

#include <cmath>

//#include "AudioProcessor.h"
#include "../CommonPlugBase.h"

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

class Oscillator// : public AudioProcessor
{
public:
	Oscillator(CommonPlugBase* pPlug);
	~Oscillator();

	double GetNextSample();

	void SetMode(int newMode);
	void SetFrequency(double frequency);

	// todo remove
	void SetSampleRate(double sampleRate);

protected:
	//void HandleParamChange(int paramType) override;

private:
	int mOscillatorMode;

	double mFrequency;
	double mSampleRate;

	double mPhasePosition;
	double mPhaseIncrement;

	void UpdatePhaseIncrement();
};

#endif // !__OSCILLATOR_H__

