#include "ScaledBitmap.h"

ScaledBitmap::ScaledBitmap(int width, int height, double scaleFactor) :
	mOutputWidthPx(width),
	mOutputHeightPx(height),
	mWidthPx(width* scaleFactor),
	mHeightPx(height* scaleFactor),
	mScaleFactor(scaleFactor),
	mWrapperBitmap(nullptr, 0, 0, 0, false),
	mOutputBitmap(nullptr, 0, 0)
{
	AllocateArrays();
}

ScaledBitmap::~ScaledBitmap()
{
	DeleteArrays();
}

void ScaledBitmap::Erase()
{
	BucketFill(LICE_RGBA(0, 0, 0, 0));
	//FillCircle(100, 50, 4, 0xff4caf50);
}

IBitmap& ScaledBitmap::GetIBitmap()
{
	ResizeImage();
	return mOutputBitmap;
}

int ScaledBitmap::GetWidth()
{
	return 0;
}

int ScaledBitmap::GetHeight()
{
	return 0;
}

void ScaledBitmap::BucketFill(int color)
{
	int size = ImageSize();
	for (int i = 0; i < size; i++) {
		mPixelData[i] = color;
	}
}

void ScaledBitmap::FillCircle(double x0, double y0, double radius, int color)
{
	x0 *= mScaleFactor;
	y0 *= mScaleFactor;
	radius *= mScaleFactor;

	int centerIndex = Index(x0, y0);

	for (int dx = 0; dx <= radius; dx++) {
		int dy = (int) round(radius * sin(acos(((double)dx) / ((double)radius))));

		int col1 = x0 - dx;
		int col2 = x0 + dx + 1;
		int row = mWidthPx * (y0 - dy);
		for (int i = 0; i < 2 * dy; i++) {
			mPixelData[row + col1] = color;
			mPixelData[row + col2] = color;
			row += mWidthPx;
		}
	}
}

void ScaledBitmap::ResizeImage()
{
	const static double ratioX = ((double) (mWidthPx - 1)) / mOutputWidthPx;
	const static double ratioY = ((double) (mHeightPx - 1)) / mOutputHeightPx;
	int outputIndex = 0;
	for (int newY = 0; newY < mOutputHeightPx; newY++) {
		for (int newX = 0; newX < mOutputWidthPx; newX++) {
			// Closest source pixel and the distance to it
			int srcX = (int)(ratioX * newX);
			int srcY = (int)(ratioY * newY);
			double dx = (ratioX * newX) - srcX;
			double dy = (ratioY * newY) - srcY;

			// Closest pixels
			int srcIndex = Index(srcX, srcY);
			int centerPx = mPixelData[srcIndex];
			int rightPx = mPixelData[srcIndex + 1];
			int downPx = mPixelData[srcIndex + mWidthPx];
			int cornerPx = mPixelData[srcIndex + mWidthPx + 1];

			// Calculate components
			int outputPx = 0;
			for (int shift = 0; shift <= 24; shift += 8) {
				int channel = (((centerPx >> shift) & 0xff) * (1 - dx) * (1 - dy))
					+ (((rightPx >> shift) & 0xff) * (dx) * (1 - dy))
					+ (((downPx >> shift) & 0xff) * (1 - dx) * (dy))
					+ (((cornerPx >> shift) & 0xff) * (dx) * (dy));
				outputPx += channel << shift;
			}

			// Re-assemble
			mOutputData[outputIndex] = outputPx;
			outputIndex++;
		}
	}
}

void ScaledBitmap::AllocateArrays()
{
	// Allocate the arrays
	mPixelData = new LICE_pixel[mWidthPx * mHeightPx];
	mOutputData = new LICE_pixel[mOutputWidthPx * mOutputHeightPx];
	// Now that we have pointers to the arrays, build the wrappers around them
	mWrapperBitmap = LICE_WrapperBitmap(mOutputData, mOutputWidthPx,
		mOutputHeightPx, mOutputWidthPx, false);
	mOutputBitmap = IBitmap(&mWrapperBitmap, mOutputWidthPx, mOutputHeightPx);
}

void ScaledBitmap::DeleteArrays()
{
	delete[] mPixelData;
	mPixelData = nullptr;
	delete[] mOutputData;
	mOutputData = nullptr;
}

inline int ScaledBitmap::ImageSize()
{
	return mWidthPx * mHeightPx;
}

inline int ScaledBitmap::Index(int x, int y)
{
	return (y * mWidthPx) + x;
}

inline int ScaledBitmap::OutputIndex(int x, int y)
{
	return (y * mOutputWidthPx) + x;
}
