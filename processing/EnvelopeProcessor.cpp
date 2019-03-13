#include "EnvelopeProcessor.h"

EnvelopeProcessor::EnvelopeProcessor(IPlugBase* pPlug) :
	AudioProcessor(pPlug, kNumParams),
	mVolumeProcessor(pPlug)
{
}

EnvelopeProcessor::~EnvelopeProcessor()
{
}

double EnvelopeProcessor::GetAdjustedSample(double sample)
{
	mVolumeProcessor.SetDecibels(CalculateCurrentAdjustment());

	// Set up the next sample
	mElapsedTime += mSecondsPerSample;
	if (IsReadyToProgress())
	{
		mElapsedTime = 0; // TODO be more precise: subtract the time setting
		mCurrentSegment++;
		if (mCurrentSegment > kReleaseSegment)
		{
			mCurrentSegment = kSilenceSegment;
		}
	}

	return mVolumeProcessor.GetAdjustedSample(sample);
}

void EnvelopeProcessor::TriggerNoteAttack()
{
	mElapsedTime = 0;
	mCurrentSegment = kAttackSegment;
}

void EnvelopeProcessor::TriggerNoteRelease()
{
	mElapsedTime = 0;
	mCurrentSegment = kReleaseSegment;
}

bool EnvelopeProcessor::IsNoteSilent()
{
	return mCurrentSegment == kSilenceSegment;
}

double EnvelopeProcessor::CalculateCurrentAdjustment()
{
	switch (mCurrentSegment)
	{
	case kSilenceSegment:
		return kMinDb;
	case kAttackSegment:
		return kMinDb + ((mElapsedTime / mAttackTime) * (kMaxDb - kMinDb));
	case kDecaySegment:
		return kMaxDb - ((mElapsedTime / mDecayTime) * (kMaxDb - mSustainLevel));
	case kSustainSegment:
		return mSustainLevel;
	case kReleaseSegment:
		return mSustainLevel - ((mElapsedTime / mReleaseTime) * (mSustainLevel - kMinDb));
	default:
		return 0;
	}
}

bool EnvelopeProcessor::IsReadyToProgress()
{
	switch (mCurrentSegment)
	{
	case kSilenceSegment:
		// This progression should be triggered by a midi event
		return false;
	case kAttackSegment:
		return mElapsedTime >= mAttackTime;
	case kDecaySegment:
		return mElapsedTime >= mDecayTime;
	case kSustainSegment:
		// This progression should be triggered by a midi event
		return false;
	case kReleaseSegment:
		return mElapsedTime >= mReleaseTime;
	default:
		// This should never happen
		return false;
	}
}

void EnvelopeProcessor::HandleParamChange(int paramType, double newValue, int newIntValue)
{
	switch (paramType)
	{
	case kAttackTimeParam:
		mAttackTime = newValue;
		break;
	case kDecayTimeParam:
		mDecayTime = newValue;
		break;
	case kSustainLevelParam:
		mSustainLevel = newValue;
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
