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
	virtual double GetFunctionValue(double x);

	virtual double GetThickFunctionTop(double x, double halfweight);
	virtual double GetThickFunctionBottom(double x, double halfweight);

private:
	const double kDefaultLineThickness = 0.01;
	const int kDefaultSubpixelResolution = 8;

	const int mWidthPx;
	const int mHeightPx;
	const double mHalfWeight;
	const int mSubpxRes;
	int mLineColor;

	unsigned char* mCellShading;
	unsigned int* mOutputRaster;
	LICE_WrapperBitmap mWrapperBitmap;
	IBitmap mOutputBitmap;

	void ShadeCells();
	void RenderPixels();

	void ClearCellShading();

	void AllocateArrays();
	void DeleteArrays();

	inline int ImageSize();
	inline int Index(int x, int y);
};

#endif // ! __FUNCTION_LINE_GRAPHIC_H__
