#include "FunctionLineGraphic.h"

FunctionLineGraphic::FunctionLineGraphic(IPlugBase* pPlug, IRECT rect, int lineColor, int fillColor) :
	IControl(pPlug, rect),
	mWidthPx(rect.W()),
	mHeightPx(rect.H()),
	mLineThickness(kDefaultLineThickness),
	mSubpxRes(kDefaultSubpixelResolution),
	mLineColor(lineColor),
	mFillColor(fillColor),
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
	ClearPositionCaches();
	CalculatePositions();
	RepairLineGaps(mTopValues, 0);
	RepairLineGaps(mBottomValues, 2);
	ClearShading();
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

void FunctionLineGraphic::ClearPositionCaches()
{
	int w_subpx = Subpx_W();
	for (int i = 0; i < w_subpx; i++) {
		mFunctionValues[i] = -1;
		mTopValues[i] = -1;
		mBottomValues[i] = -1;
	}
}

void FunctionLineGraphic::CalculatePositions()
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
		int x_func = (int)round(w_subpx * x0);
		int y_func = (int)round(w_drawpx * (y0 + halfWeight));
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

		if (x_func >= 0 && x_func < w_subpx) {
			if (mFunctionValues[x_func] == -1)
				mFunctionValues[x_func] = y_func;
			else
				mFunctionValues[x_func] = max(mFunctionValues[x_func], y_func);
			int n = mFunctionValues[x_func];
			int m = 0;
		}
		if (x_top >= 0 && x_top < w_subpx) {
			if (mTopValues[x_top] == -1)
				mTopValues[x_top] = y_top;
			else
				mTopValues[x_top] = max(mTopValues[x_top], y_top);
		}
		if (x_bot >= 0 && x_bot < w_subpx) {
			if (mBottomValues[x_bot] == -1)
				mBottomValues[x_bot] = y_bot;
			else 
				mBottomValues[x_bot] = min(mBottomValues[x_bot], y_bot);
		}

		t += kDefaultTStep;
	}
}

void FunctionLineGraphic::RepairLineGaps(int* values, int mode)
{
	// Modes: 0 for maximize, 1 for minimize, 2 for average
	int w_subpx = Subpx_W();
	for (int i = 0; i < w_subpx; i++) {
		if (values[i] == -1) {
			// Best case scenario: two adjacent values
			if (i != 0 && i != w_subpx - 1 && values[i - 1] != -1 && values[i + 1] != -1) {
				switch (mode) {
				case 0:
					values[i] = max(values[i - 1], values[i + 1]);
				case 1:
					values[i] = min(values[i - 1], values[i + 1]);
				case 2:
					values[i] = (values[i - 1] + values[i + 1]) / 2;
				}
			}
			// Lesser scenario: one adjacent value
			else if (i != 0 && values[i - 1]) {
				values[i] = values[i - 1];
			}
			else if (i != w_subpx - 1 && values[i + 1] != -1) {
				values[i] = values[i + 1];
			}
		}
	}
}

void FunctionLineGraphic::ClearShading()
{
	int size = ImageSize();
	for (int i = 0; i < size; i++) {
		mFillShading[i] = 0;
		mLineShading[i] = 0;
	}
}

void FunctionLineGraphic::ShadeCells()
{
	int w_subpx = Subpx_W();
	int h_subpx = Subpx_H();
	int maxShade = mSubpxRes * mSubpxRes;
	int index = 0;
	for (int y_bigpx = mHeightPx - 1; y_bigpx >= 0; y_bigpx--) {
		int y_subpx = y_bigpx * mSubpxRes;
		for (int x_bigpx = 0; x_bigpx < mWidthPx; x_bigpx++) {
			int x_subpx = x_bigpx * mSubpxRes;
			// Shade the fill (only checks the southwest subpixel and uses an all-or nothing shade)
			int y_func = mFunctionValues[x_subpx];
			if (y_func > y_subpx) {
				mFillShading[index] = maxShade;
			}
			// Shade the line (count the subpixels)
			for (int i = 0; i < mSubpxRes; i++) {
				int y_top = mTopValues[x_subpx + i];
				int y_bot = mBottomValues[x_subpx + i];
				int shade = min(y_top + 1, y_subpx + mSubpxRes) - max(y_bot, y_subpx);
				if (shade > 0)
					mLineShading[index] += shade;
			}
			index++;
		}
	}
}

void FunctionLineGraphic::RenderPixels()
{
	int maxShade = mSubpxRes * mSubpxRes;
	int fill_rgb = mFillColor & 0x00ffffff;
	double fill_alpha = (double)((mFillColor & 0xff000000) >> 24);
	int line_rgb = mLineColor & 0x00ffffff;
	double line_alpha = (double)((mLineColor & 0xff000000) >> 24);
	int size = ImageSize();
	for (int i = 0; i < size; i++) {
		// Reset the pixel
		mOutputRaster[i] = 0x00000000;
		int pxFill = 0;
		int pxLine = 0;
		// If there is fill here, calculate the alpha
		if (mFillShading[i] > 0) {
			pxFill = (int)(fill_alpha * ((double)mFillShading[i]) / maxShade);
		}
		// If there is line here, calculate the alpha
		if (mLineShading[i] > 0) {
			pxLine = (int)(line_alpha * ((double)mLineShading[i]) / maxShade);
		}
		// Now composite
		if (pxFill > 0 || pxLine > 0) {
			// Fastest case: there is just fill here
			if (pxLine == 0) {
				mOutputRaster[i] = (pxFill << 24) | fill_rgb;
			}
			// Second-fastest case: there is just line here
			else if (pxFill == 0) {
				mOutputRaster[i] = (pxLine << 24) | line_rgb;
			}
			// Worst case: there is both fill and line, and we need to composite
			else {
				// Calculate alpha ratios
				double fill = ((double)pxFill) / 0xff;
				double line = ((double)pxLine) / 0xff;
				fill *= (1 - line); // becasuse the line will appear on top of the fill
				int r = (fill * ((fill_rgb & 0x00ff0000) >> 16)) + (line * ((line_rgb & 0x00ff0000) >> 16));
				int g = (fill * ((fill_rgb & 0x0000ff00) >> 8)) + (line * ((line_rgb & 0x0000ff00) >> 8));
				int b = (fill * ((fill_rgb & 0x000000ff) >> 0)) + (line * ((line_rgb & 0x000000ff) >> 0));
				int pixel = (0xff << 24) | (r << 16) | (g << 8) | (b << 0);
				mOutputRaster[i] = pixel;
			}
		}
	}
}

void FunctionLineGraphic::AllocateArrays()
{
	// Allocate the line boundary caches
	int w_subpx = Subpx_W();
	mFunctionValues = new int[w_subpx];
	mTopValues = new int[w_subpx];
	mBottomValues = new int[w_subpx];
	// Allocate the cell / pixel arrays
	int size = ImageSize();
	mFillShading = new unsigned char[size];
	mLineShading = new unsigned char[size];
	mOutputRaster = new unsigned int[size];
	// Now that we have pointers to the arrays, build the wrappers around them
	mWrapperBitmap = LICE_WrapperBitmap(mOutputRaster, mWidthPx,
		mHeightPx, mWidthPx, false);
	mOutputBitmap = IBitmap(&mWrapperBitmap, mWidthPx, mHeightPx);
}

void FunctionLineGraphic::DeleteArrays()
{
	delete[] mFunctionValues;
	mFunctionValues = nullptr;
	delete[] mTopValues;
	mTopValues = nullptr;
	delete[] mBottomValues;
	mBottomValues = nullptr;
	delete[] mFillShading;
	mFillShading = nullptr;
	delete[] mLineShading;
	mLineShading = nullptr;
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
