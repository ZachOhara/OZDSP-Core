#include "EnvelopeProcessor.h"

EnvelopeProcessor::EnvelopeProcessor() :
	ModularProcessor(kNumParams)
{
	ProgressToSegment(kSilenceSegment, 0.0, 0.0, 1.0);
}

EnvelopeProcessor::~EnvelopeProcessor()
{
}

double EnvelopeProcessor::GetAdjustedSample(double sample)
{
	if (!IsInStationarySegment()) {
		double progress = 1.0 - (((double)mRemainingSamples) / ((double)mSegmentSamples));
		double scaledProgress = pow(progress, mSegmentExponent);
		double currentDelta = scaledProgress * mSegmentDifference;
		mCurrentOutput = currentDelta + mSegmentInitialOutput;
		mVolumeProcessor.SetLoudness(mCurrentOutput);
		// Update sample count and (possibly) progress the segment
		mRemainingSamples--;
		if (mRemainingSamples == 0) {
			switch (mCurrentSegment) {
			case kAttackSegment:
				ProgressToSegment(kDecaySegment, mDecayTime, mSustainLevel, mDecayExponent);
				break;
			case kDecaySegment:
				ProgressToSegment(kSustainSegment, 0.0, 0.0, 1.0);
				break;
			case kReleaseSegment:
				ProgressToSegment(kSilenceSegment, 0.0, 0.0, 1.0);
				break;
			}
		}
	}

	return mVolumeProcessor.GetAdjustedSample(sample);
}

void EnvelopeProcessor::TriggerNoteAttack()
{
	ProgressToSegment(kAttackSegment, mAttackTime, 1.0, mAttackExponent);
}

void EnvelopeProcessor::TriggerNoteRelease()
{
	ProgressToSegment(kReleaseSegment, mReleaseTime, 0.0, mReleaseExponent);
}

bool EnvelopeProcessor::IsNoteSilent()
{
	return mCurrentSegment == kSilenceSegment;
}

bool EnvelopeProcessor::IsInStationarySegment()
{
	return mCurrentSegment == kSilenceSegment || mCurrentSegment == kSustainSegment;
}

void EnvelopeProcessor::ProgressToSegment(int newSegment, double duration, double goal, double exponent)
{
	mCurrentSegment = newSegment;
	mSegmentSamples = (int) std::floor(duration / mSecondsPerSample);
	mRemainingSamples = mSegmentSamples;
	mSegmentExponent = exponent;
	if (IsInStationarySegment()) {
		mSegmentInitialOutput = 0;
		mSegmentDifference = 0;
		if (mCurrentSegment == kSilenceSegment)
			mCurrentOutput = 0.0;
		else // in sustain segment
			mCurrentOutput = mSustainLevel;
		mVolumeProcessor.SetLoudness(mCurrentOutput);
	} else {
		mSegmentInitialOutput = mCurrentOutput;
		mSegmentDifference = goal - mCurrentOutput;
	}
}

void EnvelopeProcessor::HandleParamChange(int paramType, double newValue, int newIntValue)
{
	switch (paramType) {
	case kAttackTimeParam:
		mAttackTime = newValue;
		break;
	case kDecayTimeParam:
		mDecayTime = newValue;
		break;
	case kSustainLevelParam:
		// incoming value is a percent
		mSustainLevel = newValue / 100.0;
		break;
	case kReleaseTimeParam:
		mReleaseTime = newValue;
		break;
	case kAttackShapeParam:
		mAttackExponent = GetExponentFromShapeParameter(newValue);
		break;
	case kDecayShapeParam:
		mDecayExponent = GetExponentFromShapeParameter(newValue);
		break;
	case kReleaseShapeParam:
		mReleaseExponent = GetExponentFromShapeParameter(newValue);
		break;
	}
}

void EnvelopeProcessor::HandleHostReset()
{
	mSecondsPerSample = 1.0 / GetSampleRate();
}

double EnvelopeProcessor::GetExponentFromShapeParameter(double shape)
{
	// shape is a value ranging on [-x, x] for some x
	// we want to convert it to an exponent
	if (shape >= 0) {
		return shape + 1;
	}
	else {
		return 1.0 / (abs(shape) + 1);
	}
}
