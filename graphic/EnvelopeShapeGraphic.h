#ifndef __ENVELOPE_SHAPE_GRAPHIC_H__
#define __ENVELOPE_SHAPE_GRAPHIC_H__

#include "IPlug_include_in_plug_hdr.h"

#include "../audio/EnvelopeProcessor.h"
#include "../graphic/ScaledBitmap.h"

#include <algorithm>
#include <utility>

class EnvelopeShapeGraphic : public IControl
{
public:
	EnvelopeShapeGraphic(IPlugBase* pPlug, EnvelopeProcessor* processor, IRECT rect);
	~EnvelopeShapeGraphic();

	bool Draw(IGraphics* pGraphics) override;

private:
	static const int kInnerPadPx = 3;
	// These two should be linked
	static const int kLineRadius = 2;
	static const int kRasterPadPx = 2;

	IColor mColor;

	EnvelopeProcessor* mpProcessor;

	IRECT mInnerRect;
	int mWidthPx;
	int mHeightPx;

	double* mOutputs;

	// TODO make this more efficient
	bool IsDirty() override;

	void CalculateOutputs(double* outputs, int nFrames);
	void CalculateSegmentOutput(double* outputs, int segmentStart, int segmentEnd,
		double startOutput, double endOutput, double exponent);
	void RasterizeOutputs(ScaledBitmap& scb);

	void DrawThickVerticalLine(ScaledBitmap& scb, int x, int y0, int y1, int radius);
	int GetOutputYPos(double output);

	void ZeroOutputs();

	void InitDimensions();
	void AllocateArrays();
	void DeleteArrays();
};

#endif // !__ENVELOPE_SHAPE_GRAPHIC_H__
