#include "EnvelopeProcessor.h"

EnvelopeProcessor::EnvelopeProcessor(IPlugBase* pPlug) :
	ModularProcessor(pPlug, kNumParams),
	mVolumeProcessor(pPlug)
{
}

EnvelopeProcessor::~EnvelopeProcessor()
{
}

double EnvelopeProcessor::GetAdjustedSample(double sample)
{
	if (!IsInStationarySegment()) {
		mCurrentOutput += mIncrement;
		mRemainingSamples--;
	}

	// TODO maybe only set this once for the stationary segments?
	mVolumeProcessor.SetLoudness(mCurrentOutput);

	if (!IsInStationarySegment() && mRemainingSamples == 0) {
		switch (mCurrentSegment) {
		// kSilenceSegment and kSustainSegment won't be here
		case kAttackSegment:
			ProgressSegment(kDecaySegment, mDecayTime, mSustainLevel);
			break;
		case kDecaySegment:
			ProgressSegment(kSustainSegment, 0.0, 0.0);
			break;
		case kReleaseSegment:
			ProgressSegment(kSilenceSegment, 0.0, 0.0);
			break;
		}
	}

	return mVolumeProcessor.GetAdjustedSample(sample);
}

void EnvelopeProcessor::TriggerNoteAttack()
{
	ProgressSegment(kAttackSegment, mAttackTime, 1.0);
}

void EnvelopeProcessor::TriggerNoteRelease()
{
	ProgressSegment(kReleaseSegment, mReleaseTime, 0.0);
}

bool EnvelopeProcessor::IsNoteSilent()
{
	return mCurrentSegment == kSilenceSegment;
}

bool EnvelopeProcessor::IsInStationarySegment()
{
	return mCurrentSegment == kSilenceSegment || mCurrentSegment == kSustainSegment;
}

void EnvelopeProcessor::ProgressSegment(int newSegment, double segmentDuration, double goalOutput)
{
	mCurrentSegment = newSegment;
	if (IsInStationarySegment()) {
		mRemainingSamples = -1;
		mIncrement = 0.0;
		if (mCurrentSegment == kSilenceSegment)
			mCurrentOutput = 0.0;
		else // in sustain segment
			mCurrentOutput = mSustainLevel;
	} else {
		double dLevel = goalOutput - mCurrentOutput;
		mRemainingSamples = (int) std::floor(segmentDuration / mSecondsPerSample);
		mIncrement = dLevel / mRemainingSamples;
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
		mAttackShape = newIntValue;
		break;
	case kDecayShapeParam:
		mDecayShape = newIntValue;
		break;
	case kReleaseShapeParam:
		mReleaseShape = newIntValue;
		break;
	}
}

void EnvelopeProcessor::HandleHostReset()
{
	mSecondsPerSample = 1 / GetSampleRate();
}
