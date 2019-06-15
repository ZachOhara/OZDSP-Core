#include "FunctionLineGraphic.h"

FunctionLineGraphic::FunctionLineGraphic(IPlugBase* pPlug, IRECT rect, int lineColor) :
	IControl(pPlug, rect),
	mWidthPx(rect.W()),
	mHeightPx(rect.H()),
	mLineThickness(kDefaultLineThickness),
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
	ClearLineBoundCaches();
	CalculateLineBounds();
	RepairLineGaps();
	ClearCellShading();
	ShadeCells();
	RenderPixels();
	pGraphics->DrawBitmap(&mOutputBitmap, GetRECT());
	return true;
}

double FunctionLineGraphic::GetFunctionValue(double x, double ymax)
{
	// This should be overridden in the subclass to hold the actual function
	return 0.5 * ymax;
}

double FunctionLineGraphic::GetLocalDerivative(double x, double ymax)
{
	// This should be overridden in the subclass to hold the actual function derivative
	return 0.0 * ymax;
}

void FunctionLineGraphic::ClearLineBoundCaches()
{
	int w_subpx = Subpx_W();
	for (int i = 0; i < w_subpx; i++) {
		mTopValues[i] = -1;
		mBottomValues[i] = -1;
	}
}

void FunctionLineGraphic::CalculateLineBounds()
{
	// Calculate and cache some values
	int w_subpx = Subpx_W();
	int h_subpx = Subpx_H();
	int linebuffer = ((int)ceil(mLineThickness));
	int w_drawpx = (mWidthPx - linebuffer) * mSubpxRes;
	int h_drawpx = (mHeightPx - linebuffer) * mSubpxRes;
	double heightRatio = ((double)h_drawpx) / w_drawpx;
	double halfWeight = (mLineThickness / mWidthPx) / 2;
	// Start the loop
	double t = 0.0;
	while (t < 1.0) {
		double x0 = t;
		double y0 = GetFunctionValue(x0, heightRatio);
		double local_deriv = GetLocalDerivative(x0, heightRatio);
		if (local_deriv == 0) {
			local_deriv = 0.000000001;
		}
		double orthagonal_slope = -1.0 / local_deriv;
		double dx = sqrt(pow(halfWeight, 2) / (1.0 + pow(orthagonal_slope, 2)));
		double dy = dx * orthagonal_slope;
		int x_top = (int)round(w_subpx * (x0 - dx));
		int y_top = (int)round(w_drawpx * (y0 - dy + halfWeight));
		int x_bot = (int)round(w_subpx * (x0 + dx));
		int y_bot = (int)round(w_drawpx * (y0 + dy + halfWeight));

		if (local_deriv < 0) {
			std::swap(x_top, x_bot);
			std::swap(y_top, y_bot);
		}

		y_top = min(y_top, h_subpx - 1);
		y_top = max(y_top, 0);
		y_bot = min(y_bot, h_subpx - 1);
		y_bot = max(y_bot, 0);

		if (x_top >= 0 && x_top < w_subpx) {
			if (mTopValues[x_top] == -1) {
				mTopValues[x_top] = y_top;
			}
			else {
				mTopValues[x_top] = max(mTopValues[x_top], y_top);
			}
		}
		if (x_bot >= 0 && x_bot < w_subpx) {
			if (mBottomValues[x_bot] == -1) {
				mBottomValues[x_bot] = y_bot;
			}
			else {
				mBottomValues[x_bot] = min(mBottomValues[x_bot], y_bot);
			}
		}

		if (t > 0.45 && t < 0.5)
			int n = 1;

		t += kDefaultTStep;
	}
}

void FunctionLineGraphic::RepairLineGaps()
{
	int w_subpx = Subpx_W();
	int h_subpx = Subpx_H();
	for (int i = 0; i < w_subpx; i++) {
		if (mTopValues[i] == -1)
			mTopValues[i] = h_subpx - 1;
		if (mBottomValues[i] == -1)
			mBottomValues[i] = 0;
	}
}

void FunctionLineGraphic::ClearCellShading()
{
	int size = ImageSize();
	for (int i = 0; i < size; i++) {
		mCellShading[i] = 0;
	}
}

void FunctionLineGraphic::ShadeCells()
{
	int w_subpx = Subpx_W();
	int h_subpx = Subpx_H();
	for (int x = 0; x < w_subpx; x++) {
		int y_top = mTopValues[x];
		int y_bot = mBottomValues[x];
		//if (y_top != -1 && y_bot != -1) {
			for (int y = y_bot; y <= y_top; y++) {
				int x_bigpx = (int)(((double)x) / mSubpxRes);
				int y_bigpx = (int)(((double)y) / mSubpxRes);
				mCellShading[Index(x_bigpx, mHeightPx - y_bigpx - 1)] += 1;
			}
		//}
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

void FunctionLineGraphic::AllocateArrays()
{
	// Allocate the line boundary caches
	int w_subpx = Subpx_W();
	mTopValues = new int[w_subpx];
	mBottomValues = new int[w_subpx];
	// Allocate the cell / pixel arrays
	int size = ImageSize();
	mCellShading = new unsigned char[size];
	mOutputRaster = new unsigned int[size];
	// Now that we have pointers to the arrays, build the wrappers around them
	mWrapperBitmap = LICE_WrapperBitmap(mOutputRaster, mWidthPx,
		mHeightPx, mWidthPx, false);
	mOutputBitmap = IBitmap(&mWrapperBitmap, mWidthPx, mHeightPx);
}

void FunctionLineGraphic::DeleteArrays()
{
	delete[] mTopValues;
	mTopValues = nullptr;
	delete[] mBottomValues;
	mBottomValues = nullptr;
	delete[] mCellShading;
	mCellShading = nullptr;
	delete[] mOutputRaster;
	mOutputRaster = nullptr;
}

inline int FunctionLineGraphic::Subpx_W()
{
	return mWidthPx * mSubpxRes;
}

inline int FunctionLineGraphic::Subpx_H()
{
	return mHeightPx * mSubpxRes;
}

inline int FunctionLineGraphic::ImageSize()
{
	return mWidthPx * mHeightPx;
}

inline int FunctionLineGraphic::Index(int x, int y)
{
	return (y * mWidthPx) + x;
}
