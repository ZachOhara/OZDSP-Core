#include "FunctionLineGraphic.h"

FunctionLineGraphic::FunctionLineGraphic(IPlugBase* pPlug, IRECT rect, int lineColor) :
	IControl(pPlug, rect),
	mWidthPx(rect.W()),
	mHeightPx(rect.H()),
	mHalfWeight(kDefaultLineThickness / 2),
	mSubpxRes(kDefaultSubpixelResolution),
	mLineColor(lineColor),
	mWrapperBitmap(nullptr, 0, 0, 0, false),
	mOutputBitmap(nullptr, 0, 0)
{
	AllocateArrays();
}

FunctionLineGraphic::~FunctionLineGraphic()
{
	DeleteArrays();
}

bool FunctionLineGraphic::Draw(IGraphics* pGraphics)
{
	ClearCellShading();
	ShadeCells();
	RenderPixels();
	pGraphics->DrawBitmap(&mOutputBitmap, GetRECT());
	return true;
}

double FunctionLineGraphic::GetFunctionValue(double x)
{
	// This should be overridden in the subclass to hold the actual function
	return 0.5;
}

double FunctionLineGraphic::GetThickFunctionTop(double x, double halfweight)
{
	return GetFunctionValue(x + halfweight) + halfweight;
}

double FunctionLineGraphic::GetThickFunctionBottom(double x, double halfweight)
{
	return GetFunctionValue(x - halfweight) - halfweight;
}

void FunctionLineGraphic::ShadeCells()
{
	// This assumes the cells have already been cleared
	int w_subpx = mWidthPx * mSubpxRes;
	int h_subpx = mHeightPx * mSubpxRes;
	for (int x = 0; x < w_subpx; x++) {
		double progress = ((double)x) / w_subpx;
		int y0 = floor(h_subpx * GetThickFunctionBottom(progress, mHalfWeight));
		int y1 = ceil(h_subpx * GetThickFunctionTop(progress, mHalfWeight));
		y0 = min(y0, h_subpx - 1);
		y0 = max(y0, 0);
		y1 = min(y1, h_subpx - 1);
		y1 = max(y1, 0);

		for (int y = y0; y <= y1; y++) {
			int x_bigpx = (int)(((double)x) / mSubpxRes);
			int y_bigpx = (int)(((double)y) / mSubpxRes);
			mCellShading[Index(x_bigpx, mHeightPx - y_bigpx - 1)] += 1;
		}
	}
}

void FunctionLineGraphic::RenderPixels()
{
	int max_pxval = mSubpxRes * mSubpxRes;
	int rgb = mLineColor & 0x00ffffff;
	double alpha = (double)((mLineColor & 0xff000000) >> 24);
	int size = ImageSize();
	for (int i = 0; i < size; i++) {
		// optimization: if there is no data at this index, skip all the math
		if (mCellShading[i] == 0) {
			mOutputRaster[i] = 0x00000000;
			continue;
		}
		int newalpha = (int)(alpha * ((double)mCellShading[i]) / max_pxval);
		mOutputRaster[i] = (newalpha << 24) | rgb;
	}
}

void FunctionLineGraphic::ClearCellShading()
{
	int size = ImageSize();
	for (int i = 0; i < size; i++) {
		mCellShading[i] = 0;
	}
}

void FunctionLineGraphic::AllocateArrays()
{
	int size = ImageSize();
	// Allocate the arrays
	mCellShading = new unsigned char[size];
	mOutputRaster = new unsigned int[size];
	// Now that we have pointers to the arrays, build the wrappers around them
	mWrapperBitmap = LICE_WrapperBitmap(mOutputRaster, mWidthPx,
		mHeightPx, mWidthPx, false);
	mOutputBitmap = IBitmap(&mWrapperBitmap, mWidthPx, mHeightPx);
}

void FunctionLineGraphic::DeleteArrays()
{
	delete[] mCellShading;
	mCellShading = nullptr;
	delete[] mOutputRaster;
	mOutputRaster = nullptr;
}

inline int FunctionLineGraphic::ImageSize()
{
	return mWidthPx * mHeightPx;
}

inline int FunctionLineGraphic::Index(int x, int y)
{
	return (y * mWidthPx) + x;
}
