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
	//DrawThickLine(0, 50, 100, 75, 2, 0xffaaaaaa);
	//WuThickLine(0, 50, 100, 25, 3, 0x88ff00ff);
	//WuThickLine(100, 25, 115, 85, 3, 0x88ff00ff);
	//WuThickLine(0, 50, 100, 25, 3, 0xffffffff);
	//WuThickLine(100, 25, 115, 85, 3, 0xffffffff);
}

IBitmap& ScaledBitmap::GetIBitmap()
{
	//GaussBlur();
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

void ScaledBitmap::DrawThickLine(double x0, double y0, double x1, double y1, double thickness, int color)
{
	x0 *= mScaleFactor;
	y0 *= mScaleFactor;
	x1 *= mScaleFactor;
	y1 *= mScaleFactor;
	thickness *= mScaleFactor;

	// TEST ONLY
	plotLineWidth(x0, y0, x1, y1, thickness);

	int dx = (int)abs(x1 - x0);
	int dy = (int)abs(y1 - y0);
	int sx = x0 < x1 ? 1 : -1;
	int sy = y0 < y1 ? 1 : -1;
	
	int err = dx - dy, e2, x2, y2;                          /* error value e_xy */
	float ed = dx + dy == 0 ? 1 : sqrt((float)dx * dx + (float)dy * dy);

	for (thickness = (thickness + 1) / 2; ; ) {                                   /* pixel loop */
		//setPixelColor(x0, y0, max(0, 255 * (abs(err - dx + dy) / ed - wd + 1)));
		if (x0 >= 0 && x0 < mWidthPx && y0 >= 0 && y0 < mHeightPx)
			mPixelData[Index(x0, y0)] = color;
		e2 = err; x2 = x0;
		if (2 * e2 >= -dx) {                                           /* x step */
			for (e2 += dy, y2 = y0; e2 < ed * thickness && (y1 != y2 || dx > dy); e2 += dx)
				//setPixelColor(x0, y2 += sy, max(0, 255 * (abs(e2) / ed - wd + 1)));
				if (x0 >= 0 && x0 < mWidthPx && y2 + sy >= 0 && y2 + sy < mHeightPx)
					mPixelData[Index(x0, y2 += sy)] = color;
			if (x0 == x1) break;
			e2 = err; err -= dy; x0 += sx;
		}
		if (2 * e2 <= dy) {                                            /* y step */
			for (e2 = dx - e2; e2 < ed * thickness && (x1 != x2 || dx < dy); e2 += dy)
				//setPixelColor(x2 += sx, y0, max(0, 255 * (abs(e2) / ed - wd + 1)));
				if (x2 + sx >= 0 && x2 + sx < mWidthPx && y0 >= 0 && y0 < mHeightPx)
					mPixelData[Index(x2 += sx, y0)] = color;
			if (y0 == y1) break;
			err += dx; y0 += sy;
		}
	}
}

void ScaledBitmap::GaussBlur()
{
	static const int kernelSize = 7;
	static const int kernelRadius = (kernelSize - 1) / 2;

	// Comment out one of these:
	// Sigma = 0.84
	/*
	static const double kernel[kernelSize][kernelSize] = {
		{0.00000067, 0.00002292, 0.00019117, 0.00038771, 0.00019117, 0.00002292, 0.00000067},
		{0.00002292, 0.00078633, 0.00655965, 0.01330373, 0.00655965, 0.00078633, 0.00002292},
		{0.00019117, 0.00655965, 0.05472157, 0.11098164, 0.05472157, 0.00655965, 0.00019117},
		{0.00038771, 0.01330373, 0.11098164, 0.22508352, 0.11098164, 0.01330373, 0.00038771},
		{0.00019117, 0.00655965, 0.05472157, 0.11098164, 0.05472157, 0.00655965, 0.00019117},
		{0.00002292, 0.00078633, 0.00655965, 0.01330373, 0.00655965, 0.00078633, 0.00002292},
		{0.00000067, 0.00002292, 0.00019117, 0.00038771, 0.00019117, 0.00002292, 0.00000067},
	};
	//*/
	// Sigma = 3.0
	///*
	static const double kernel[kernelSize][kernelSize] = {
		{0.011362, 0.014962, 0.017649, 0.018648, 0.017649, 0.014962, 0.011362},
		{0.014962, 0.019703, 0.023240, 0.024556, 0.023240, 0.019703, 0.014962},
		{0.017649, 0.023240, 0.027413, 0.028964, 0.027413, 0.023240, 0.017649},
		{0.018648, 0.024556, 0.028964, 0.030603, 0.028964, 0.024556, 0.018648},
		{0.017649, 0.023240, 0.027413, 0.028964, 0.027413, 0.023240, 0.017649},
		{0.014962, 0.019703, 0.023240, 0.024556, 0.023240, 0.019703, 0.014962},
		{0.011362, 0.014962, 0.017649, 0.018648, 0.017649, 0.014962, 0.011362},
	};
	//*/
	for (int x = 0; x < mWidthPx; x++) {
		for (int y = 0; y < mHeightPx; y++) {
			if (mPixelData[Index(x, y)]) {

				for (int kx = -kernelRadius; kx <= kernelRadius; kx++) {
					if (x + kx >= 0 && x + kx < mWidthPx) {
						for (int ky = -kernelRadius; ky <= kernelRadius; ky++) {
							if (y + ky >= 0 && y + ky < mHeightPx) {

								mPixelData[Index(x + kx, y + ky)] += kernel[kx + kernelRadius][ky + kernelRadius];
							}
						}
					}
				}

			}
		}
	}
}

void ScaledBitmap::WuThickLine(int x0, int y0, int x1, int y1, int thickness, int color)
{
	int rgb = color & 0x00ffffff;
	int alpha = (color & 0xff000000) >> 24;
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
			int y_ipart = (int)floor(y);
			double y_fpart = y - y_ipart;
			double y_rfpart = 1 - y_fpart;

			/*
			int curralpha = (mPixelData[Index(x, y_ipart)] & 0xff000000) >> 24;
			int newalpha = max(curralpha, (int)(255 * y_rfpart));
			int newpx = rgb | (newalpha<<24);
			mPixelData[Index(x, y_ipart)] = newpx;
			*/
			MaximizeAlpha(x, y_ipart, rgb, alpha * y_rfpart);

			for (int t = 1; t < thickness; t++) {
				mPixelData[Index(x, y_ipart + t)] = (alpha << 24) | rgb;
			}

			/*
			curralpha = (mPixelData[Index(x, y_ipart + thickness)] & 0xff000000) >> 24;
			newalpha = max(curralpha, (int)(255 * y_fpart));
			newpx = rgb | (newalpha << 24);
			mPixelData[Index(x, y_ipart + thickness)] = newpx;
			*/
			MaximizeAlpha(x, y_ipart + thickness, rgb, alpha * y_fpart);

			y += slope;
		}
	} else {
		for (int x = x0; x <= x1; x++) {
			int y_ipart = (int)floor(y);
			double y_fpart = y - y_ipart;
			double y_rfpart = 1 - y_fpart;

			//mPixelData[Index(y_ipart, x)] = LICE_RGBA(255, 255, 255, (int)(255 * y_rfpart));
			MaximizeAlpha(y_ipart, x, rgb, alpha * y_rfpart);
			for (int t = 1; t < thickness; t++) {
				mPixelData[Index(y_ipart + t, x)] = (alpha << 24) | rgb;
			}
			//mPixelData[Index(y_ipart + thickness, x)] = LICE_RGBA(255, 255, 255, (int)(255 * y_fpart));
			MaximizeAlpha(y_ipart + thickness, x, rgb, alpha * y_fpart);
			y += slope;
		}
	}
}

void ScaledBitmap::MaximizeAlpha(int x, int y, int rgb, int newalpha)
{
	int curralpha = (mPixelData[Index(x, y)] & 0xff000000) >> 24;
	newalpha = max(curralpha, newalpha);
	int newpx = rgb | (newalpha << 24);
	mPixelData[Index(x, y)] = newpx;
}

void ScaledBitmap::plotLineWidth(int x0, int y0, int x1, int y1, float wd)
{
	int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
	int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
	int err = dx - dy, e2, x2, y2;                          /* error value e_xy */
	float ed = dx + dy == 0 ? 1 : sqrt((float)dx * dx + (float)dy * dy);

	for (wd = (wd + 1) / 2; ; ) {                                   /* pixel loop */
		//setPixelColor(x0, y0, max(0, 255 * (abs(err - dx + dy) / ed - wd + 1)));
		int intensity = max(0, 255 * (abs(err - dx + dy) / ed - wd + 1));
		if (intensity > 0) {
			int n = 0;
			n += 2;
		}
		mPixelData[Index(x0, y0)] = LICE_RGBA(255, 255, 255, intensity);
		//
		e2 = err; x2 = x0;
		if (2 * e2 >= -dx) {                                           /* x step */
			for (e2 += dy, y2 = y0; e2 < ed * wd && (y1 != y2 || dx > dy); e2 += dx)
				//setPixelColor(x0, y2 += sy, max(0, 255 * (abs(e2) / ed - wd + 1)));
				int intensity = max(0, 255 * (abs(e2) / ed - wd + 1));
				mPixelData[Index(x0, y2 += sy)] = LICE_RGBA(255, 255, 255, intensity);
				//

			if (x0 == x1) break;
			e2 = err; err -= dy; x0 += sx;
		}
		if (2 * e2 <= dy) {                                            /* y step */
			for (e2 = dx - e2; e2 < ed * wd && (x1 != x2 || dx < dy); e2 += dy)
				//setPixelColor(x2 += sx, y0, max(0, 255 * (abs(e2) / ed - wd + 1)));
				int intensity = max(0, 255 * (abs(e2) / ed - wd + 1));
				mPixelData[Index(x2 += sx, y0)] = LICE_RGBA(255, 255, 255, intensity);
			if (y0 == y1) break;
			err += dx; y0 += sy;
		}
	}
}

void ScaledBitmap::FillCircle(double x0, double y0, double radius, int color)
{
	color = 0xffaaaaaa;
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

			//int outputPx = ResampleBicubic(srcX, srcY, dx, dy);
			// TEST ONLY!!!!!
			int outputPx = mPixelData[outputIndex];

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
