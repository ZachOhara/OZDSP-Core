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

			int outputPx = ResampleBicubic(srcX, srcY, dx, dy);

			// Re-assemble
			mOutputData[outputIndex] = outputPx;
			outputIndex++;
		}
	}
}

inline int ScaledBitmap::ResampleBilinear(int srcX, int srcY, double dx, double dy)
{
	// Find closest pixels
	int srcIndex = (srcY * mWidthPx) + srcX;
	int centerPx = mPixelData[srcIndex];
	int rightPx = mPixelData[srcIndex + 1];
	int downPx = mPixelData[srcIndex + mWidthPx];
	int cornerPx = mPixelData[srcIndex + mWidthPx + 1];

	// If there is no data here, circumvent the math
	if (!(centerPx | rightPx | downPx | cornerPx))
		return 0;

	// Calculate components
	int outputPx = 0;
	for (int shift = 0; shift <= 24; shift += 8) {
		int channel = (((centerPx >> shift) & 0xff) * (1 - dx) * (1 - dy))
			+ (((rightPx >> shift) & 0xff) * (dx) * (1 - dy))
			+ (((downPx >> shift) & 0xff) * (1 - dx) * (dy))
			+ (((cornerPx >> shift) & 0xff) * (dx) * (dy));
		outputPx += channel << shift;
	}

	return outputPx;
}

inline int ScaledBitmap::ResampleBicubic(int srcX, int srcY, double dx, double dy)
{
	if (srcX < 1)
		srcX = 1;
	if (srcY < 1)
		srcY = 1;

	// Find closest pixels
	int srcIndex = (srcY * mWidthPx) + srcX;

	int row = srcIndex - mWidthPx;

	int p00 = mPixelData[row - 1];
	int p10 = mPixelData[row];
	int p20 = mPixelData[row + 1];
	int p30 = mPixelData[row + 2];

	row += mWidthPx;
	int p01 = mPixelData[row - 1];
	int p11 = mPixelData[row];
	int p21 = mPixelData[row + 1];
	int p31 = mPixelData[row + 2];

	row += mWidthPx;
	int p02 = mPixelData[row - 1];
	int p12 = mPixelData[row];
	int p22 = mPixelData[row + 1];
	int p32 = mPixelData[row + 2];

	row += mWidthPx;
	int p03 = mPixelData[row - 1];
	int p13 = mPixelData[row];
	int p23 = mPixelData[row + 1];
	int p33 = mPixelData[row + 2];

	// Attempt to circumvent the math if possible
	if (!(p00 | p10 | p20 | p30 
		| p01 | p11 | p21 | p31
		| p02 | p12 | p22 | p32
		| p03 | p13 | p23 | p33))
		return 0;

	int outputPx = 0;
	for (int shift = 0; shift <= 24; shift += 8) {
		double col0 = InterpolateCubic((p00 >> shift) & 0xff, (p10 >> shift) & 0xff, (p20 >> shift) & 0xff, (p30 >> shift) & 0xff, dx);
		double col1 = InterpolateCubic((p01 >> shift) & 0xff, (p11 >> shift) & 0xff, (p21 >> shift) & 0xff, (p31 >> shift) & 0xff, dx);
		double col2 = InterpolateCubic((p02 >> shift) & 0xff, (p12 >> shift) & 0xff, (p22 >> shift) & 0xff, (p32 >> shift) & 0xff, dx);
		double col3 = InterpolateCubic((p03 >> shift) & 0xff, (p13 >> shift) & 0xff, (p23 >> shift) & 0xff, (p33 >> shift) & 0xff, dx);
		int channel = (int)InterpolateCubic(col0, col1, col2, col3, dy);
		if (channel < 0)
			channel = 0;
		if (channel > 255)
			channel = 255;
		outputPx += channel << shift;
	}

	return outputPx;
}

inline double ScaledBitmap::InterpolateCubic(double a, double b, double c, double d, double t)
{
	double a0 = -a / 2.0 + (3.0 * b) / 2.0 - (3.0 * c) / 2.0 + d / 2.0;
	double b0 = a - (5.0 * b) / 2.0 + 2.0 * c - d / 2.0;
	double c0 = -a / 2.0 + c / 2.0;
	double d0 = b;

	double ret = (a0 * t * t * t) + (b0 * t * t) + (c0 * t) + d0;
	//if (ret > 255) throw 1;
	//if (ret < 0) throw 1;
	return a0 * t* t* t + b0 * t * t + c0 * t + d0;
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
