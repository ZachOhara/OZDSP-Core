#ifndef __ENVELOPE_PROCESSOR_H__
#define __ENVELOPE_PROCESSOR_H__

#include "IPlug_include_in_plug_hdr.h"

#include "AudioProcessor.h"
#include "VolumeProcessor.h"

class EnvelopeProcessor : public AudioProcessor
{
public:
	enum EParameters {kAttackTimeParam, kDecayTimeParam, kSustainLevelParam, kReleaseTimeParam,
		kAttackShapeParam, kDecayShapeParam, kReleaseShapeParam, kNumParams};
	enum EEnvelopeShapes {kLinearShape, kExponentialShape, kLogarithmicShape};

	EnvelopeProcessor(IPlugBase* pPlug);
	~EnvelopeProcessor();

	double GetAdjustedSample(double sample);

	void TriggerNoteAttack();
	void TriggerNoteRelease();
	bool IsNoteSilent();

protected:
	void HandleParamChange(int paramType, double newValue, int newIntValue) override;
	void HandleHostReset() override;

private:
	enum ESegments {kSilenceSegment, kAttackSegment, kDecaySegment, kSustainSegment, kReleaseSegment};

	const double kMinDb = -100.0;
	const double kMaxDb = 0.0;

	VolumeProcessor mVolumeProcessor;

	double mSecondsPerSample;

	double mElapsedTime;
	int mCurrentSegment;

	double mAttackTime;
	double mDecayTime;
	double mSustainLevel;
	double mReleaseTime;
	int mAttackShape;
	int mDecayShape;
	int mReleaseShape;

	double CalculateCurrentAdjustment();
	bool IsReadyToProgress();
};

#endif // !__ENVELOPE_PROCESSOR_H__
