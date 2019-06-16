#ifndef __ENVELOPE_SHAPE_GRAPHIC_H__
#define __ENVELOPE_SHAPE_GRAPHIC_H__

#include "IPlug_include_in_plug_hdr.h"

#include "../graphic/FunctionLineGraphic.h"

#include <algorithm>
#include <utility>

class EnvelopeShapeGraphic : public FunctionLineGraphic
{
public:
	EnvelopeShapeGraphic(IPlugBase* pPlug, IRECT rect, int lineColor, int fillColor);
	~EnvelopeShapeGraphic();

	bool IsDirty() override;

	void TriggerRedraw(double attackTimeNorm, double decayTimeNorm, double sustainLevel, double releaseTimeNorm,
		double peakLevel, double attackExponent, double decayExponent, double releaseExponent);

protected:
	double GetFunctionValue(double x, double ymax) override;
	double GetLocalDerivative(double x, double ymax) override;

private:
	bool isDirty = false;

	double mSustainLevel;
	double mPeakLevel;

	double mAttackRbound;
	double mDecayRbound;
	double mReleaseLbound;

	double mAttackExponent;
	double mDecayExponent;
	double mReleaseExponent;
};

#endif // !__ENVELOPE_SHAPE_GRAPHIC_H__
