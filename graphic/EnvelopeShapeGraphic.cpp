#include "EnvelopeShapeGraphic.h"

EnvelopeShapeGraphic::EnvelopeShapeGraphic(IPlugBase* pPlug, IRECT rect, int lineColor, int fillColor) :
	FunctionLineGraphic(pPlug, rect, lineColor, fillColor)
{
}

EnvelopeShapeGraphic::~EnvelopeShapeGraphic()
{
}

bool EnvelopeShapeGraphic::IsDirty()
{
	return isDirty;
}

void EnvelopeShapeGraphic::TriggerRedraw(double attackTimeNorm, double decayTimeNorm,
	double sustainLevel, double releaseTimeNorm, double peakLevel,
	double attackExponent, double decayExponent, double releaseExponent)
{
	isDirty = true;

	mSustainLevel = sustainLevel;
	mPeakLevel = peakLevel;

	mAttackRbound = attackTimeNorm * 0.25;
	mDecayRbound = mAttackRbound + (decayTimeNorm * 0.25);
	mReleaseLbound = 1 - (releaseTimeNorm * 0.25);

	mAttackExponent = attackExponent;
	mDecayExponent = decayExponent;
	mReleaseExponent = releaseExponent;
}

double EnvelopeShapeGraphic::GetFunctionValue(double x, double ymax)
{
	isDirty = false; // since this method is only called by the Draw() method

	if (x < mAttackRbound) {
		double seg_progress = x / mAttackRbound;
		return ymax * mPeakLevel * pow(seg_progress, mAttackExponent);
	} else if (x < mDecayRbound) {
		double seg_progress = (x - mAttackRbound) / (mDecayRbound - mAttackRbound);
		return ymax * (mPeakLevel - ((mPeakLevel - mSustainLevel) * pow(seg_progress, mDecayExponent)));
	} else if (x < mReleaseLbound) {
		return ymax * mSustainLevel;
	} else {
		double seg_progress = (x - mReleaseLbound) / (1 - mReleaseLbound);
		return ymax * (mSustainLevel - (mSustainLevel * pow(seg_progress, mReleaseExponent)));
	}
}

double EnvelopeShapeGraphic::GetLocalDerivative(double x, double ymax)
{
	if (x < mAttackRbound) {
		double seg_progress = x / mAttackRbound;
		return mPeakLevel * mAttackExponent * pow(seg_progress, mAttackExponent - 1);
	}
	else if (x < mDecayRbound) {
		double seg_progress = (x - mAttackRbound) / (mDecayRbound - mAttackRbound);
		return -0.5 * (mPeakLevel - mSustainLevel) * mDecayExponent * pow(seg_progress, mDecayExponent - 1);
	}
	else if (x < mReleaseLbound) {
		return 0;
	}
	else {
		double seg_progress = (x - mReleaseLbound) / (1 - mReleaseLbound);
		return -0.5 * mSustainLevel * mReleaseExponent * pow(seg_progress, mReleaseExponent - 1);
	}
}
