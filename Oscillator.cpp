#include "Oscillator.h"

Oscillator::Oscillator() :
	mOscillatorMode(kModeSine),
	mPhasePosition(0)
{
}

Oscillator::~Oscillator()
{
}

void Oscillator::SetMode(int newMode)
{
	mOscillatorMode = newMode;
}

void Oscillator::SetFrequency(double frequency)
{
	mFrequency = frequency;
	UpdatePhaseIncrement();
}

void Oscillator::SetSampleRate(double sampleRate)
{
	mSampleRate = sampleRate;
	UpdatePhaseIncrement();
}

double Oscillator::GetNextSample()
{
	double sampleValue = 0;
	switch (mOscillatorMode)
	{
	case kModeSine:
		sampleValue = sin(2 * M_PI * mPhasePosition);
		break;
	case kModeTriangle:
		if (mPhasePosition < 0.5)
		{
			sampleValue = 1 - (4 * abs(mPhasePosition - 0.25));
		}
		else
		{
			sampleValue = (4 * abs(mPhasePosition - 0.75)) - 1;
		}
		break;
	case kModeSquare:
		if (mPhasePosition < 0.5)
		{
			sampleValue = 1;
		}
		else
		{
			sampleValue = -1;
		}
		break;
	case kModeSawtooth:
		sampleValue = 1 - (2 * mPhasePosition);
		break;
	default:
		sampleValue = 0;
		break;
	}

	// Update the phase for the next cycle
	mPhasePosition += mPhaseIncrement;
	if (mPhasePosition > 1)
	{
		mPhasePosition -= 1;
	}

	return sampleValue;
}

void Oscillator::UpdatePhaseIncrement()
{
	// don't update phase position, because we actually want phase to be conserved if the frequency changes
	// that way we won't hear a nasty clip as the phase resets
	mPhaseIncrement = mFrequency / mSampleRate;
}
