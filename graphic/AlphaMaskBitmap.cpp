#include "AlphaMaskBitmap.h"

AlphaMaskBitmap::AlphaMaskBitmap(int width, int height) :
	mWidthPx(width),
	mHeightPx(height),
	mWrapperBitmap(nullptr, 0, 0, 0, false),
	mOutputBitmap(nullptr, 0, 0)
{
	AllocateArrays();
}

AlphaMaskBitmap::~AlphaMaskBitmap()
{
	DeleteArrays();
}

void AlphaMaskBitmap::Erase()
{
	BucketFill(0);
	DrawThickLine(0, 50, 100, 75, 3);
}

IBitmap& AlphaMaskBitmap::GetColoredImage(int color)
{
	int rgb = color & 0x00ffffff;
	double alpha = (double)((color & 0xff000000) >> 24);
	int size = ImageSize();
	for (int i = 0; i < size; i++) {
		int m = mMaskData[i];
		double mask_alpha = ((double)mMaskData[i]) / 255.0;
		char newalpha = (char)(alpha * mask_alpha);
		mOutputData[i] = (newalpha << 24) | rgb;
		if (m != 0)
			m += 1;
	}
	return mOutputBitmap;
}

void AlphaMaskBitmap::BucketFill(char alpha)
{
	int size = ImageSize();
	for (int i = 0; i < size; i++) {
		mMaskData[i] = alpha;
	}
}

void AlphaMaskBitmap::DrawThickLine(int x0, int y0, int x1, int y1, int thickness)
{
	// This uses a modified version of Xiolin Wu's line algorithm

	bool isSteep = abs(y1 - y0) > abs(x1 - x0);
	if (isSteep) {
		std::swap(x0, y0);
		std::swap(x1, y1);
	}
	if (x0 > x1) {
		std::swap(x0, x1);
		std::swap(y0, y0);
	}

	int dx = x1 - x0;
	int dy = y1 - y0;
	double slope = ((double)dy) / dx;
	double y = y0 - (((double)thickness) / 2);

	if (!isSteep) {
		for (int x = x0; x <= x1; x++) {
			int y_ipart = floor(y);
			double y_fpart = y - y_ipart;
			double y_rfpart = 1 - y_fpart;
			mMaskData[Index(x, y_ipart)] = max(mMaskData[Index(x, y_ipart)], 0xff * y_rfpart);
			for (int t = 1; t < thickness; t++) {
				mMaskData[Index(x, y_ipart + t)] = 0xff;
			}
			mMaskData[Index(x, y_ipart + thickness)] = max(mMaskData[Index(x, y_ipart + thickness)], 0xff * y_fpart);
			y += slope;
		}
	} else {
		for (int x = x0; x <= x1; x++) {
			int y_ipart = floor(y);
			double y_fpart = y - y_ipart;
			double y_rfpart = 1 - y_fpart;
			mMaskData[Index(y_ipart, x)] = max(mMaskData[Index(y_ipart, x)], 0xff * y_rfpart);
			for (int t = 1; t < thickness; t++) {
				mMaskData[Index(y_ipart + t, x)] = 0xff;
			}
			mMaskData[Index(y_ipart + thickness, x)] = max(mMaskData[Index(y_ipart + thickness, x)], 0xff * y_fpart);
			y += slope;
		}
	}
}

void AlphaMaskBitmap::AllocateArrays()
{
	// Allocate the arrays
	int size = ImageSize();
	mMaskData = new char[size];
	mOutputData = new int[size];
	// Now that we have pointers to the arrays, build the wrappers around them
	mWrapperBitmap = LICE_WrapperBitmap((LICE_pixel*)mOutputData, mWidthPx,
		mHeightPx, mWidthPx, false);
	mOutputBitmap = IBitmap(&mWrapperBitmap, mWidthPx, mHeightPx);
}

void AlphaMaskBitmap::DeleteArrays()
{
	delete[] mMaskData;
	mMaskData = nullptr;
	delete[] mOutputData;
	mOutputData = nullptr;
}

inline int AlphaMaskBitmap::ImageSize()
{
	return mWidthPx * mHeightPx;
}

inline int AlphaMaskBitmap::Index(int x, int y)
{
	return (y * mWidthPx) + x;
}
