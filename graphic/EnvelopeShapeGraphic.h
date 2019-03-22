#ifndef __ENVELOPE_SHAPE_GRAPHIC_H__
#define __ENVELOPE_SHAPE_GRAPHIC_H__

#include "IPlug_include_in_plug_hdr.h"

#include "../audio/EnvelopeProcessor.h"

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
	bool* mRaster;
	//double* mGaussRaster;
	double* mFxaaRaster;

	// TODO make this more efficient
	bool IsDirty() override;

	void CalculateOutputs(double* outputs, int nFrames);
	void CalculateSegmentOutput(double* outputs, int segmentStart, int segmentEnd,
		double startOutput, double endOutput, double exponent);
	void RasterizeOutputs();
	void DrawRaster(IGraphics* pGraphics);

	void DrawThickVerticalLine(bool* raster, int x, int y0, int y1, int radius);
	void FillCircle(bool* raster, int x0, int y0, int radius);
	void DrawCircle(bool* raster, int x0, int y0, int radius);
	int GetOutputYPos(double output);

	//void GaussBlur(bool* raster, double* gauss);
	void AntialiasFxaa(bool* inputs, double* outputs);

	inline int dim_zerobound(int n) { return (n < 0 ? 0 : n); };
	inline int dim_widthbound(int n) { return (n >= mWidthPx ? mWidthPx - 1 : n); };
	inline int dim_heightbound(int n) { return (n >= mHeightPx ? mHeightPx - 1 : n); };

	inline double abs_dbl(double n) { return (n >= 0 ? n : -n); };
	inline double min_dbl(double a, double b) { return (a < b ? a : b); };
	inline double max_dbl(double a, double b) { return (a > b ? a : b); };
	inline double getLuma(bool input) { return (input ? 1.0 : 0.0); };

	void ZeroOutputs();
	void ZeroRasters();

	void InitDimensions();
	void AllocateArrays();
	void DeleteArrays();

	inline int Index(int x, int y);

	/*
	EnvelopeProcessor* mpProcessor;

	IColor mColor;
	IChannelBlend mDividerBlend;
	IChannelBlend mPathBlend;

	static const int kNumSegments = 4;
	static const int kNumDividers = kNumSegments - 1;

	static const int kInnerPadPx = 3;
	static const int kDividerWidthPx = 4;
	static const int kTraceWidthPx = 2;

	IRECT mSegmentDividers[kNumDividers];

	IRECT mPaddedRect;

	int mMaxSegmentWidth;

	int mTotalWidth;
	int mTotalHeight;
	double** mBrightnessRaster; // TODO convert to bool, then blur?
	*/

	/*
	void DrawSegmentPath(IGraphics* pGraphics, int segment,
		double startOutput, double endOutput, double exponent);
	void DrawPathDot(IGraphics* pGraphics, int lastX, int lastY, int x, int y);

	int GetSegmentBoundL(int segment);
	int GetSegmentBoundR(int segment);
	int GetOutputPosition(double output);

	void InitDimensions();
	void InitDividers();

	void DrawRaster(IGraphics* pGraphics);
	void AllocateRaster();
	void ZeroRaster();
	void DeleteRaster();
	*/

	/*
	void MidpointCircle(double** raster, int x0, int y0, int radius);

	void WuDrawLine(double** raster, double x0, double y0, double x1, double y1);

	inline double Wu_ipart(double x) { return std::floor(x); };
	inline double Wu_round(double x) { return std::round(x); };
	inline double Wu_fpart(double x) { return x - std::floor(x); };
	inline double Wu_rfpart(double x) { return 1 - Wu_fpart(x); };
	inline void Wu_plot(double** raster, int x, int y, double value) { raster[x][y] = value; };
	*/
};

#endif // !__ENVELOPE_SHAPE_GRAPHIC_H__
