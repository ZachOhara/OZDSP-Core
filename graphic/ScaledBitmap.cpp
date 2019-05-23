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
	FillColor(255, 0, 0, 255);
}

IBitmap& ScaledBitmap::GetIBitmap()
{
	ResizeImage();
	return mOutputBitmap;
}

void ScaledBitmap::FillColor(int r, int g, int b, int a)
{
	LICE_pixel pixel = LICE_RGBA(r, g, b, a);
	int size = ImageSize();
	for (int i = 0; i < size; i++) {
		mPixelData[i] = pixel;
	}
	/*
	for (int x = 0; x < mWidthPx; x++) {
		for (int y = 0; y < mHeightPx; y++) {
			mPixelData[Index(x, y)] = LICE_RGBA(r, g, b, a);
		}
	}
	*/
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
			int centerPx = (int)mPixelData[srcIndex];
			int rightPx = (int)mPixelData[srcIndex + 1];
			int downPx = (int)mPixelData[srcIndex + mWidthPx];
			int cornerPx = (int)mPixelData[srcIndex + mWidthPx + 1];

			// Calculate components
			int channels[4]; // {blue, green, red, alpha}
			for (int i = 0; i < 4; i++) {
				int shift = i * 8;
				channels[i] = (((centerPx >> shift) & 0xff) * (1 - dx) * (1 - dy))
					+ (((rightPx >> shift) & 0xff) * (dx) * (1 - dy))
					+ (((downPx >> shift) & 0xff) * (1 - dx) * (dy))
					+ (((cornerPx >> shift) & 0xff) * (dx) * (dy));
			}

			/*
			int blue = ((centerPx & 0xff) * (1 - dx) * (1 - dy))
				+ ((rightPx & 0xff) * (dx) * (1 - dy))
				+ ((downPx & 0xff) * (1 - dx) * (dy))
				+ ((cornerPx & 0xff) * (dx) * (dy));

			int green = (((centerPx >> 8) & 0xff) * (1 - dx) * (1 - dy))
				+ (((rightPx >> 8) & 0xff) * (dx) * (1 - dy))
				+ (((downPx >> 8) & 0xff) * (1 - dx) * (dy))
				+ (((cornerPx >> 8) & 0xff) * (dx) * (dy));

			int red = (((centerPx >> 16) & 0xff) * (1 - dx) * (1 - dy))
				+ (((rightPx >> 16) & 0xff) * (dx) * (1 - dy))
				+ (((downPx >> 16) & 0xff) * (1 - dx) * (dy))
				+ (((cornerPx >> 16) & 0xff) * (dx) * (dy));

			int alpha = (((centerPx >> 24) & 0xff) * (1 - dx) * (1 - dy))
				+ (((rightPx >> 24) & 0xff) * (dx) * (1 - dy))
				+ (((downPx >> 24) & 0xff) * (1 - dx) * (dy))
				+ (((cornerPx >> 24) & 0xff) * (dx) * (dy));
			*/

			// Re-assemble
			//mOutputData[outputIndex] = LICE_RGBA(red, green, blue, alpha);
			mOutputData[outputIndex] = LICE_RGBA(channels[2], channels[1], channels[0], channels[3]);
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
