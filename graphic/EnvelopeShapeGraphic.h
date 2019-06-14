#ifndef __ENVELOPE_SHAPE_GRAPHIC_H__
#define __ENVELOPE_SHAPE_GRAPHIC_H__

#include "IPlug_include_in_plug_hdr.h"

#include "../audio/EnvelopeProcessor.h"
#include "../graphic/AlphaMaskBitmap.h"
#include "../graphic/FunctionLineGraphic.h"

#include <algorithm>
#include <utility>

class EnvelopeShapeGraphic : public FunctionLineGraphic
{
public:
	EnvelopeShapeGraphic(IPlugBase* pPlug, EnvelopeProcessor* processor, IRECT rect);
	~EnvelopeShapeGraphic();

	bool IsDirty() override;

protected:
	double GetFunctionValue(double x) override;

	double GetThickFunctionTop(double x, double halfweight) override;
	double GetThickFunctionBottom(double x, double halfweight) override;

private:
	EnvelopeProcessor* mpProcessor;

	double AttackOutput(double x);
	double DecayOutput(double x);
	double SustainOutput(double x);
	double ReleaseOutput(double x);
};

#endif // !__ENVELOPE_SHAPE_GRAPHIC_H__
