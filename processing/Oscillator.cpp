#include "Oscillator.h"

Oscillator::Oscillator(IPlugBase* pPlug) :
	AudioProcessor(pPlug, kNumParams),
	mWaveform(kSineWave),
	mFrequency(0),
	mPhasePosition(0),
	mPhaseIncrement(0)
{
}

Oscillator::~Oscillator()
{
}

double Oscillator::GetNextSample()
{
	double sampleValue = 0;
	switch (mWaveform) {
	case kSineWave:
		sampleValue = sin(2 * M_PI * mPhasePosition);
		break;
	case kTriangleWave:
		if (mPhasePosition < 0.5)
			sampleValue = 1 - (4 * abs(mPhasePosition - 0.25));
		else
			sampleValue = (4 * abs(mPhasePosition - 0.75)) - 1;
		break;
	case kSquareWave:
		sampleValue = (mPhasePosition < 0.5 ? 1.0 : -1.0);
		break;
	case kSawtoothWave:
		sampleValue = 1 - (2 * mPhasePosition);
		break;
	default:
		sampleValue = 0;
		break;
	}

	// Update the phase for the next cycle
	mPhasePosition += mPhaseIncrement;
	if (mPhasePosition > 1)
		mPhasePosition -= 1;

	return sampleValue;
}

void Oscillator::SetWaveform(int waveform)
{
	mWaveform = waveform;
}

void Oscillator::SetFrequency(double frequency)
{
	mFrequency = frequency;
	UpdatePhaseIncrement();
}

void Oscillator::HandleParamChange(int paramType, double newValue, int newIntValue)
{
	switch (paramType) {
	case kFrequencyParam:
		SetFrequency(newValue);
		break;
	case kWaveformParam:
		SetWaveform(newIntValue);
		break;
	}
}

void Oscillator::HandleHostReset()
{
	mPhasePosition = 0.0;
	UpdatePhaseIncrement();
}

void Oscillator::UpdatePhaseIncrement()
{
	// Don't update phase position, because we want phase to be conserved if the frequency changes
	// That way we won't hear a clip as the phase resets
	mPhaseIncrement = mFrequency / GetSampleRate();
}
