#ifndef __OSCILLATOR__
#define __OSCILLATOR__

#include <cmath>

#define M_PI 3.14159265358979323846

enum OscillatorMode {
	kModeSine,
	kModeTriangle,
	kModeSquare,
	kModeSawtooth,
	kNumOscillatorModes
};

class Oscillator
{
public:
	Oscillator();
	~Oscillator();

	void SetMode(int newMode);
	void SetFrequency(double frequency);
	void SetSampleRate(double sampleRate);

	double GetNextSample();

private:
	int mOscillatorMode;

	double mFrequency;
	double mSampleRate;

	double mPhasePosition;
	double mPhaseIncrement;

	void UpdatePhaseIncrement();
};

#endif // !__OSCILLATOR__

