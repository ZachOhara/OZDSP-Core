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
	const int kBoxPadPx = 3;
	const double kScaleFactor = 4.0;
	const  double kLineWidth = 1.0;

	ScaledBitmap mBitmap;
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

	void DrawThickVerticalLine(double x, double y0, double y1, double radius);

	void AllocateArray();
	void DeleteArrays();
};

#endif // !__ENVELOPE_SHAPE_GRAPHIC_H__
