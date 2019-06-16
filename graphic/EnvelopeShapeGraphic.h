#ifndef __ENVELOPE_SHAPE_GRAPHIC_H__
#define __ENVELOPE_SHAPE_GRAPHIC_H__

#include "IPlug_include_in_plug_hdr.h"

#include "../audio/EnvelopeProcessor.h"
#include "../graphic/FunctionLineGraphic.h"

#include <algorithm>
#include <utility>

class EnvelopeShapeGraphic : public FunctionLineGraphic
{
public:
	EnvelopeShapeGraphic(IPlugBase* pPlug, IRECT rect, EnvelopeProcessor* pProcessor,
		int lineColor, int fillColor);
	~EnvelopeShapeGraphic();

	bool IsDirty() override;

protected:
	double GetFunctionValue(double x, double ymax) override;
	double GetLocalDerivative(double x, double ymax) override;

private:
	EnvelopeProcessor* mpProcessor;
};

#endif // !__ENVELOPE_SHAPE_GRAPHIC_H__
