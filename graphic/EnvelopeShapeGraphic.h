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

	//bool Draw(IGraphics* pGraphics) override;

protected:
	double GetFunctionValue(double x) override;

private:
	const int kBoxPadPx = 3;
	const int kLineWidth = 1;

	AlphaMaskBitmap mBitmap;
	int mColor;

	EnvelopeProcessor* mpProcessor;

	double* mOutputs;
	int nOutputFrames;

	// TODO make this more efficient
	bool IsDirty() override;

	void CalculateOutputs(double* outputs, int nFrames);
	void CalculateSegmentOutput(double* outputs, int segmentStart, int segmentEnd,
		double startOutput, double endOutput, double exponent);
	void RasterizeOutputs();

	void AllocateArray();
	void DeleteArrays();
};

#endif // !__ENVELOPE_SHAPE_GRAPHIC_H__
