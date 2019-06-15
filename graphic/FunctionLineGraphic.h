#ifndef __FUNCTION_LINE_GRAPHIC_H__
#define __FUNCTION_LINE_GRAPHIC_H__

#include "IPlug_include_in_plug_hdr.h"

#include <cmath>
#include <minmax.h>

class FunctionLineGraphic : public IControl
{
public:
	FunctionLineGraphic(IPlugBase* pPlug, IRECT rect, int lineCcolor);
	~FunctionLineGraphic();

	bool Draw(IGraphics* pGraphics) override;

protected:
	virtual double GetFunctionValue(double x, double ymax);
	virtual double GetLocalDerivative(double x, double ymax);

private:
	const double kDefaultLineThickness = 3.0;
	const int kDefaultSubpixelResolution = 8;
	const double kDefaultTStep = 0.0004;

	const int mWidthPx;
	const int mHeightPx;
	const double mLineThickness;
	const int mSubpxRes;
	int mLineColor;

	int* mTopValues;
	int* mBottomValues;

	unsigned char* mCellShading;
	unsigned int* mOutputRaster;
	LICE_WrapperBitmap mWrapperBitmap;
	IBitmap mOutputBitmap;

	void ClearLineBoundCaches();
	void CalculateLineBounds();
	void RepairLineGaps();
	void ClearCellShading();
	void ShadeCells();
	void RenderPixels();

	void AllocateArrays();
	void DeleteArrays();

	inline int Subpx_W();
	inline int Subpx_H();
	inline int ImageSize();
	inline int Index(int x, int y);
};

#endif // ! __FUNCTION_LINE_GRAPHIC_H__
