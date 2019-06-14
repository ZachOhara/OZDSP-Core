#ifndef  __SCALED_BITMAP_H__
#define __SCALED_BITMAP_H__

#include "IPlug_include_in_plug_hdr.h"

#include <cmath>
#include <algorithm>
#include <minmax.h>

class ScaledBitmap
{
public:
	ScaledBitmap(int width, int height, double scaleFactor);
	~ScaledBitmap();

	void Erase();
	IBitmap& GetIBitmap();

	int GetWidth();
	int GetHeight();

	// Drawing methods
	void BucketFill(int color);
	void DrawThickLine(double x0, double y0, double x1, double y1, double thickness, int color);
	void GaussBlur();
	void FillCircle(double x0, double y0, double radius, int color);

	// TEST ONLY
	void WuThickLine(int x0, int y0, int x1, int y1, int thickness, int color);

private:
	const int mOutputWidthPx;
	const int mOutputHeightPx;
	const int mWidthPx;
	const int mHeightPx;
	const double mScaleFactor;

	LICE_pixel* mPixelData;
	LICE_pixel* mOutputData;
	LICE_WrapperBitmap mWrapperBitmap;
	IBitmap mOutputBitmap;

	// TEST ONLY
	//void WuThickLine(int x0, int y0, int x1, int y1, int thickness, int color);
	void MaximizeAlpha(int x, int y, int rgb, int newalpha);
	void plotLineWidth(int x0, int y0, int x1, int y1, float wd);

	void ResizeImage();
	inline int ResampleBilinear(int srcX, int srcY, double dx, double dy);
	inline int ResampleBicubic(int srcX, int srcY, double dx, double dy);
	static inline double InterpolateCubic(double a, double b, double c, double d, double t);

	void AllocateArrays();
	void DeleteArrays();

	inline int ImageSize();
	inline int Index(int x, int y);
	inline int OutputIndex(int x, int y);
};

#endif // ! __SCALED_BITMAP_H__
