#ifndef  __ALPHA_MASK_BITMAP_H__
#define __ALPHA_MASK_BITMAP_H__

#include "IPlug_include_in_plug_hdr.h"

#include <cmath>
#include <algorithm>
#include <minmax.h>

class AlphaMaskBitmap
{
public:
	AlphaMaskBitmap(int width, int height);
	~AlphaMaskBitmap();

	void Erase();
	IBitmap& GetColoredImage(int color);

	// Drawing methods
	void BucketFill(char alpha);
	void DrawThickLine(int x0, int y0, int x1, int y1, int thickness);

private:
	const int mWidthPx;
	const int mHeightPx;

	char* mMaskData;
	int* mOutputData;
	LICE_WrapperBitmap mWrapperBitmap;
	IBitmap mOutputBitmap;

	void AllocateArrays();
	void DeleteArrays();

	inline int ImageSize();
	inline int Index(int x, int y);
};

#endif // ! __ALPHA_MASK_BITMAP_H__
