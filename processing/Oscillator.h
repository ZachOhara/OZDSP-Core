#ifndef __OSCILLATOR_H__
#define __OSCILLATOR_H__

#include "IPlug_include_in_plug_hdr.h"

#include "AudioProcessor.h"

#include <cmath>

#define M_PI 3.14159265358979323846

class Oscillator : public AudioProcessor
{
public:
	enum EParameters {kFrequencyParam, kWaveformParam, kNumParams};
	enum EWaveforms {kSineWave, kTriangleWave, kSquareWave, kSawtoothWave, kNumWaveforms};

	Oscillator(IPlugBase* pPlug);
	~Oscillator();

	double GetNextSample();

	void SetWaveform(int waveform);
	void SetFrequency(double frequency);

protected:
	void HandleParamChange(int paramType, double newValue, int newIntValue) override;
	void HandleHostReset() override;

private:
	int mWaveform;
	double mFrequency;

	double mPhasePosition;
	double mPhaseIncrement;

	void UpdatePhaseIncrement();
};

#endif // !__OSCILLATOR_H__
