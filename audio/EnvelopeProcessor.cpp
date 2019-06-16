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
		if (mCurrentOutput == 0) {
			// TODO find out what this does
			GetExponentFromShapeParameter(0.0);
		}
		mVolumeProcessor.SetLoudness(mCurrentOutput);
		// Update sample count
		mRemainingSamples--;
		// Progress the segment
		if (mRemainingSamples == 0) {
			switch (mCurrentSegment) {
				// no case is necessary for silence or sustains, since these will
				// be progressed by external events (note attacks and releases)
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
	ProgressToSegment(kAttackSegment, mAttackTime, mPeakLevel, mAttackExponent);
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
		// also update the peak level
		mPeakLevel = GetPeakLevelFromRatio(mSustainLevel, mPeakRatio);
		break;
	case kReleaseTimeParam:
		mReleaseTime = newValue;
		break;
	case kPeakLevelParam:
		// incoming value is a percent
		mPeakRatio = newValue / 100.0;
		mPeakLevel = GetPeakLevelFromRatio(mSustainLevel, mPeakRatio);
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
	// shape is a linear value ranging on [-x, x] for some x
	// we want to convert it to an exponent on [1/x, x+1]
	if (shape >= 0) {
		return shape + 1;
	}
	else {
		return 1.0 / (abs(shape) + 1);
	}
}

double EnvelopeProcessor::GetPeakLevelFromRatio(double sustainLevel, double peakRatio)
{
	return sustainLevel + (peakRatio * (1 - sustainLevel));
}
