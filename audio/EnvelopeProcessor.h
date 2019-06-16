#ifndef __ENVELOPE_PROCESSOR_H__
#define __ENVELOPE_PROCESSOR_H__

#include "../ModularProcessor.h"
#include "VolumeProcessor.h"
#include "../graphic/EnvelopeShapeGraphic.h"

#include <cmath>

class EnvelopeProcessor : public ModularProcessor
{
public:
	enum EParameters {kAttackTimeParam, kDecayTimeParam, kSustainLevelParam, kReleaseTimeParam,
		kPeakLevelParam, kAttackShapeParam, kDecayShapeParam, kReleaseShapeParam, kNumParams};

	EnvelopeProcessor();
	~EnvelopeProcessor();

	void AttachGraphic(EnvelopeShapeGraphic* pGraphic);

	double GetAdjustedSample(double sample);

	void TriggerNoteAttack();
	void TriggerNoteRelease();
	bool IsNoteSilent();

protected:
	void HandleParamChange(int paramType, double newValue, int newIntValue) override;
	void HandleHostReset() override;

private:
	enum ESegments {kSilenceSegment, kAttackSegment, kDecaySegment, kSustainSegment, kReleaseSegment};

	VolumeProcessor mVolumeProcessor;

	EnvelopeShapeGraphic* mpGraphic = nullptr;

	double mSecondsPerSample;

	int mCurrentSegment;
	int mSegmentSamples;
	int mRemainingSamples;
	double mSegmentExponent;
	double mSegmentInitialOutput;
	double mSegmentDifference;
	double mCurrentOutput;

	double mAttackTime;
	double mDecayTime;
	double mSustainLevel;
	double mReleaseTime;
	double mPeakRatio;
	double mPeakLevel;
	double mAttackExponent;
	double mDecayExponent;
	double mReleaseExponent;

	bool IsInStationarySegment();
	void ProgressToSegment(int newSegment, double duration, double goal, double exponent);

	static double GetExponentFromShapeParameter(double shape);
	static double GetPeakLevelFromRatio(double sustainLevel, double peakRatio);
};

#endif // !__ENVELOPE_PROCESSOR_H__
