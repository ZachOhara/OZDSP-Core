#include "EnvelopeShapeGraphic.h"

EnvelopeShapeGraphic::EnvelopeShapeGraphic(IPlugBase* pPlug, EnvelopeProcessor* pProcessor, IRECT rect) :
	IControl(pPlug, rect),
	mColor(0xFF, 0x4C, 0xAF, 0x50)
{
	mpProcessor = pProcessor;
	InitDimensions();
	AllocateArrays();
}
EnvelopeShapeGraphic::~EnvelopeShapeGraphic()
{
	DeleteArrays();
}

bool EnvelopeShapeGraphic::Draw(IGraphics* pGraphics)
{
	// zero outputs
	ZeroOutputs();
	// calculate outputs
	CalculateOutputs(mOutputs, mWidthPx);

	// TODO remove this, this is just a rudimentary proof of concept
	// But this code draws a dot on every point in the output
	/*
	int left = mInnerRect.L;
	int top = mInnerRect.T;
	for (int x = 0; x < mWidthPx; x++) {
		int y = (1.0 - mOutputs[x]) * mHeightPx;
		pGraphics->FillCircle(&mColor, left + x, top + y, 1);
	}
	*/

	
	// zero raster
	ZeroRasters();
	// rasterize outputs
	RasterizeOutputs();
	// apply blur
	//GaussBlur(mRaster, mGaussRaster);
	// Apply antialiasing
	AntialiasFxaa(mRaster, mFxaaRaster);
	// draw raster
	DrawRaster(pGraphics);
	
	return true;
}

bool EnvelopeShapeGraphic::IsDirty()
{
	return true;
}

void EnvelopeShapeGraphic::CalculateOutputs(double* outputs, int nFrames)
{
	try {
		int maxSegmentWidth = nFrames / 4;
		// Current state information (all time values are normalized to [0, 1])
		double attackTime = mpProcessor->GetParamNormalized(EnvelopeProcessor::kAttackTimeParam);
		double decayTime = mpProcessor->GetParamNormalized(EnvelopeProcessor::kDecayTimeParam);
		double sustainLevel = mpProcessor->GetParamNormalized(EnvelopeProcessor::kSustainLevelParam);
		double releaseTime = mpProcessor->GetParamNormalized(EnvelopeProcessor::kReleaseTimeParam);
		double attackExponent = EnvelopeProcessor::GetExponentFromShapeParameter(
			mpProcessor->GetParamValue(EnvelopeProcessor::kAttackShapeParam));
		double decayExponent = EnvelopeProcessor::GetExponentFromShapeParameter(
			mpProcessor->GetParamValue(EnvelopeProcessor::kDecayShapeParam));
		double releaseExponent = EnvelopeProcessor::GetExponentFromShapeParameter(
			mpProcessor->GetParamValue(EnvelopeProcessor::kReleaseShapeParam));

		// Measure segment widths
		int attackRBound = attackTime * maxSegmentWidth;
		int decayRBound = attackRBound + (decayTime * maxSegmentWidth);
		int releaseLBound = nFrames - (releaseTime * maxSegmentWidth);

		// Calculate the data
		CalculateSegmentOutput(outputs, 0, attackRBound, 0.0, 1.0, attackExponent);
		CalculateSegmentOutput(outputs, attackRBound, decayRBound, 1.0, sustainLevel, decayExponent);
		CalculateSegmentOutput(outputs, decayRBound, releaseLBound, sustainLevel, sustainLevel, 1.0);
		CalculateSegmentOutput(outputs, releaseLBound, nFrames, sustainLevel, 0.0, releaseExponent);

		// Correct for a display artifact caused by steep slopes
		outputs[std::max(attackRBound, kRasterPadPx - 1)] = 1.0;
	} catch (std::exception& e) {}
}

void EnvelopeShapeGraphic::CalculateSegmentOutput(double* outputs, int segmentStart, int segmentEnd, double startOutput, double endOutput, double exponent)
{
	int width = segmentEnd - segmentStart;
	double diff = endOutput - startOutput;
	for (int i = 0; i < width; i++) {
		double progress = ((double)i) / ((double)width);
		double scaledProgress = pow(progress, exponent);
		double currentOutput = startOutput + (diff * scaledProgress);
		int x = i + segmentStart;
		outputs[x] = currentOutput;
	}
}

void EnvelopeShapeGraphic::RasterizeOutputs()
{
	int paddedBoundL = kRasterPadPx;
	int paddedBoundR = mWidthPx - kRasterPadPx;

	int outputFloor = GetOutputYPos(0.0) - 1;
	// Draw the left bound
	int firstLeftOutput = GetOutputYPos(mOutputs[paddedBoundL - 1]);
	DrawThickVerticalLine(mRaster, paddedBoundL - 1, outputFloor,
		firstLeftOutput, kLineRadius);
	// Draw the right bound
	int lastRightOutput = GetOutputYPos(mOutputs[paddedBoundR - 1]);
	int rightOutputFloor = std::max(outputFloor, lastRightOutput);
	// the above line corrects for an artifact that happens when sustain level is zero
	DrawThickVerticalLine(mRaster, paddedBoundR, lastRightOutput,
		rightOutputFloor, kLineRadius);

	// Draw the rest of the shape
	int lastY = GetOutputYPos(mOutputs[paddedBoundL - 1]);
	for (int x = paddedBoundL; x < paddedBoundR; x++) {
		int y = GetOutputYPos(mOutputs[x]);
		DrawThickVerticalLine(mRaster, x, lastY, y, kLineRadius);
		lastY = y;
	}
}

void EnvelopeShapeGraphic::DrawRaster(IGraphics* pGraphics)
{
	int left = mInnerRect.L;
	int top = mInnerRect.T;
	for (int x = 0; x < mWidthPx; x++) {
		for (int y = 0; y < mHeightPx; y++) {
			//double brightness = mGaussRaster[Index(x, y)];
			bool brightness = mRaster[Index(x, y)];
			if (brightness) {
				IColor shadedColor = IColor(0xFF, mColor.R * brightness, mColor.G * brightness, mColor.B * brightness);
				pGraphics->ForcePixel(&shadedColor, left + x, top + y);
			}
		}
	}
}

void EnvelopeShapeGraphic::DrawThickVerticalLine(bool* raster, int x, int y0, int y1, int radius)
{
	if (y1 < y0)
		std::swap(y0, y1);
	for (int y = y0; y <= y1; y++) {
		FillCircle(raster, x, y, radius);
	}
}

void EnvelopeShapeGraphic::FillCircle(bool* raster, int x0, int y0, int radius)
{
	for (int r = 1; r <= radius; r++) {
		DrawCircle(raster, x0, y0, r);
	}
}

void EnvelopeShapeGraphic::DrawCircle(bool* raster, int x0, int y0, int radius)
{
	int x = radius - 1;
	int y = 0;
	int dx = 1;
	int dy = 1;
	int err = dx - (radius << 1);

	while (x >= y)
	{
		raster[Index(x0 + x, y0 + y)] = true;
		raster[Index(x0 + y, y0 + x)] = true;
		raster[Index(x0 - y, y0 + x)] = true;
		raster[Index(x0 - x, y0 + y)] = true;
		raster[Index(x0 - x, y0 - y)] = true;
		raster[Index(x0 - y, y0 - x)] = true;
		raster[Index(x0 + y, y0 - x)] = true;
		raster[Index(x0 + x, y0 - y)] = true;

		if (err <= 0)
		{
			y++;
			err += dy;
			dy += 2;
		}

		if (err > 0)
		{
			x--;
			dx += 2;
			err += dx - (radius << 1);
		}
	}
}

int EnvelopeShapeGraphic::GetOutputYPos(double output)
{
	return ((1.0 - output) * (mHeightPx - (2 * kRasterPadPx))) + kRasterPadPx;
}

void EnvelopeShapeGraphic::ZeroOutputs()
{
	for (int i = 0; i < mWidthPx; i++) {
		mOutputs[i] = 0;
	}
}

void EnvelopeShapeGraphic::ZeroRasters()
{
	int size = mWidthPx * mHeightPx;
	for (int i = 0; i < size; i++) {
		mRaster[i] = false;
		//mGaussRaster[i] = 0.0;
		mFxaaRaster[i] = 0.0;
	}
}

void EnvelopeShapeGraphic::InitDimensions()
{
	mInnerRect = IRECT(GetRECT()->L + kInnerPadPx, GetRECT()->T + kInnerPadPx,
		GetRECT()->R - kInnerPadPx, GetRECT()->B - kInnerPadPx);
	mWidthPx = mInnerRect.R - mInnerRect.L;
	mHeightPx = mInnerRect.B - mInnerRect.T;
}

void EnvelopeShapeGraphic::AllocateArrays()
{
	mOutputs = new double[mWidthPx];
	mRaster = new bool[mWidthPx * mHeightPx];
	//mGaussRaster = new double[mWidthPx * mHeightPx];
	mFxaaRaster = new double[mWidthPx * mHeightPx];
}

void EnvelopeShapeGraphic::DeleteArrays()
{
	delete[] mOutputs;
	mOutputs = nullptr;
	delete[] mRaster;
	mRaster = nullptr;
	//delete[] mGaussRaster;
	//mGaussRaster = nullptr;
	delete[] mFxaaRaster;
	mFxaaRaster = nullptr;
}

inline int EnvelopeShapeGraphic::Index(int x, int y)
{
	return (x * mHeightPx) + y;
}

void EnvelopeShapeGraphic::AntialiasFxaa(bool* inputs, double* outputs)
{
	for (int x = 0; x < mWidthPx; x++) {
		for (int y = 0; y < mHeightPx; y++) {
			double lumaCenter = (inputs[Index(x, y)] ? 1.0 : 0.0);

			double lumaUp = getLuma(inputs[Index(x, dim_zerobound(y-1))]);
			double lumaDown = getLuma(inputs[Index(x, dim_heightbound(y+1))]);
			double lumaLeft = getLuma(inputs[Index(dim_zerobound(x - 1), y)]);
			double lumaRight = getLuma(inputs[Index(dim_widthbound(x + 1), y)]);

			double minLuma = min_dbl(lumaCenter, min_dbl(
				min_dbl(lumaUp, lumaDown), min_dbl(lumaLeft, lumaRight)));
			double maxLuma = max_dbl(lumaCenter, max_dbl(
				max_dbl(lumaUp, lumaDown), max_dbl(lumaLeft, lumaRight)));
			double rangeLuma = maxLuma - minLuma;

			const static double kEdgeThresholdMin = 0.312;
			const static double kEdgeThresholdMax = 0.125;
			if (rangeLuma < max_dbl(kEdgeThresholdMin, maxLuma * kEdgeThresholdMax)) {
				outputs[Index(x, y)] = (inputs[Index(x, y)] ? 1.0 : 0.0);
				continue;
			}

			double lumaDownLeft = getLuma(inputs[Index(dim_zerobound(x - 1), dim_heightbound(y + 1))]);
			double lumaUpLeft = getLuma(inputs[Index(dim_zerobound(x - 1), dim_zerobound(y - 1))]);
			double lumaDownRight = getLuma(inputs[Index(dim_widthbound(x + 1), dim_heightbound(y + 1))]);
			double lumaUpRight = getLuma(inputs[Index(dim_widthbound(x + 1), dim_zerobound(y - 1))]);

			double lumaDownUp = lumaDown + lumaUp;
			double lumaLeftRight = lumaLeft + lumaRight;

			double lumaLeftCorners = lumaDownLeft + lumaUpLeft;
			double lumaRightCorners = lumaDownRight + lumaUpRight;
			double lumaDownCorners = lumaDownLeft + lumaDownRight;
			double lumaUpCorners = lumaUpLeft + lumaUpRight;

			double edgeHorizontal = abs_dbl(-2.0 * lumaLeft + lumaLeftCorners) + abs_dbl(-2.0 * lumaCenter + lumaDownUp) * 2.0 + abs_dbl(-2.0 * lumaRight + lumaRightCorners);
			double edgeVertical = abs_dbl(-2.0 * lumaUp + lumaUpCorners) + abs_dbl(-2.0 * lumaCenter + lumaLeftRight) * 2.0 + abs_dbl(-2.0 * lumaDown + lumaDownCorners);
			bool isHorizontal = (edgeHorizontal >= edgeVertical);

			double luma1 = isHorizontal ? lumaDown : lumaLeft;
			double luma2 = isHorizontal ? lumaUp : lumaRight;
			// Compute gradients in this direction.
			double gradient1 = luma1 - lumaCenter;
			double gradient2 = luma2 - lumaCenter;

			// Which direction is the steepest ?
			bool is1Steepest = abs_dbl(gradient1) >= abs_dbl(gradient2);

			// Gradient in the corresponding direction, normalized.
			double gradientScaled = 0.25*max_dbl(abs_dbl(gradient1), abs_dbl(gradient2));
		}
	}
}

/*
void EnvelopeShapeGraphic::GaussBlur(bool* raster, double* gauss)
{
	static const int kernelSize = 7;
	static const int kernelRadius = (kernelSize - 1) / 2;
	
	// Comment out one of these:

	/*
	// Sigma = 0.84
	static const double kernel[kernelSize][kernelSize] = {
		{0.00000067, 0.00002292, 0.00019117, 0.00038771, 0.00019117, 0.00002292, 0.00000067},
		{0.00002292, 0.00078633, 0.00655965, 0.01330373, 0.00655965, 0.00078633, 0.00002292},
		{0.00019117, 0.00655965, 0.05472157, 0.11098164, 0.05472157, 0.00655965, 0.00019117},
		{0.00038771, 0.01330373, 0.11098164, 0.22508352, 0.11098164, 0.01330373, 0.00038771},
		{0.00019117, 0.00655965, 0.05472157, 0.11098164, 0.05472157, 0.00655965, 0.00019117},
		{0.00002292, 0.00078633, 0.00655965, 0.01330373, 0.00655965, 0.00078633, 0.00002292},
		{0.00000067, 0.00002292, 0.00019117, 0.00038771, 0.00019117, 0.00002292, 0.00000067},
	};
	
	
	// Sigma = 3.0
	static const double kernel[kernelSize][kernelSize] = {
		{0.011362, 0.014962, 0.017649, 0.018648, 0.017649, 0.014962, 0.011362},
		{0.014962, 0.019703, 0.023240, 0.024556, 0.023240, 0.019703, 0.014962},
		{0.017649, 0.023240, 0.027413, 0.028964, 0.027413, 0.023240, 0.017649},
		{0.018648, 0.024556, 0.028964, 0.030603, 0.028964, 0.024556, 0.018648},
		{0.017649, 0.023240, 0.027413, 0.028964, 0.027413, 0.023240, 0.017649},
		{0.014962, 0.019703, 0.023240, 0.024556, 0.023240, 0.019703, 0.014962},
		{0.011362, 0.014962, 0.017649, 0.018648, 0.017649, 0.014962, 0.011362},
	};



	for (int x = 0; x < mWidthPx; x++) {
		for (int y = 0; y < mHeightPx; y++) {
			if (raster[Index(x, y)]) {
				//gauss[Index(x, y)] = 1.0;
				
				for (int kx = -kernelRadius; kx <= kernelRadius; kx++) {
					if (x + kx >= 0 && x + kx < mWidthPx) {
						for (int ky = -kernelRadius; ky <= kernelRadius; ky++) {
							if (y + ky >= 0 && y + ky < mHeightPx) {
								mGaussRaster[Index(x + kx, y + ky)] += kernel[kx + kernelRadius][ky + kernelRadius];
							}
						}
					}
				}
				
			}
		}
	}
}
//*/

/*

bool EnvelopeShapeGraphic::Draw(IGraphics* pGraphics)
{
	/*
	for (int i = 0; i < kNumDividers; i++) {
		pGraphics->FillIRect(&mColor, &mSegmentDividers[i], &mDividerBlend);
	}
	*/

	/*
	ZeroRaster();
	DrawSegmentPath(pGraphics, 0, 0.0, 1.0, 3.0);
	DrawSegmentPath(pGraphics, 1, 1.0, 0.5, 3.0);
	DrawSegmentPath(pGraphics, 2, 0.5, 0.5, 1.0);
	DrawSegmentPath(pGraphics, 3, 0.5, 0.0, 1.0);
	//DrawRaster(pGraphics); 
	return true;
}

void EnvelopeShapeGraphic::DrawSegmentPath(IGraphics* pGraphics, int segment,
	double startOutput, double endOutput, double exponent)
{
	int left = GetSegmentBoundL(segment);
	int right = GetSegmentBoundR(segment);
	int width = right - left;
	double outputDiff = endOutput - startOutput;
	int lastY = GetOutputPosition(startOutput);
	for (int x = 0; x < width; x++) {
		double progress = ((double)x) / ((double)width);
		progress = pow(progress, exponent);
		double currentOutput = startOutput + (progress * outputDiff);
		int y = GetOutputPosition(currentOutput);
		DrawPathDot(pGraphics, left + x - 1, lastY, left + x, y);
		lastY = y;
	}
	DrawPathDot(pGraphics, right - 1, lastY, right, GetOutputPosition(endOutput));
}

void EnvelopeShapeGraphic::DrawPathDot(IGraphics* pGraphics, int lastX, int lastY, int x, int y)
{
	//pGraphics->FillCircle(&mColor, x, y, ((float)kTraceWidthPx) / 2, &mPathBlend, true);
	// pGraphics->DrawLine(&mColor, lastX, lastY, x, y, &mPathBlend, true);

	// Version 2
	
	int dX = x - lastX;
	int dY = y - lastY;
	int length = (int)std::round(std::pow(std::pow(dX, 2) + std::pow(dY, 2), 0.5));
	for (int i = 0; i < length; i++) {
		double progress = ((double)i) / ((double)length);
		int dotX = lastX + (progress * dX);
		int dotY = lastY + (progress * dY);
		pGraphics->FillCircle(&mColor, dotX, dotY, ((float)kTraceWidthPx) / 2, &mPathBlend, true);
	}
	

	// Version 3
	/*
	int left = mPaddedRect.L;
	int top = mPaddedRect.T;
	// TODO fix this shit
	if (lastX < 0)
		lastX = 0;
	// -- end --
	WuDrawLine(mBrightnessRaster, lastX-left, lastY-top, x-left, y-top);
	//WuDrawLine(mBrightnessRaster, lastX - left, lastY - top + 1, x - left, y - top + 1);
	//WuDrawLine(mBrightnessRaster, lastX - left, lastY - top + 2, x - left, y - top + 2);
	//

	// Version 4
	//MidpointCircle(mBrightnessRaster, left, top, 5);
}

int EnvelopeShapeGraphic::GetSegmentBoundL(int segment)
{
	return mPaddedRect.L + (segment * mMaxSegmentWidth);
}

int EnvelopeShapeGraphic::GetSegmentBoundR(int segment)
{
	return GetSegmentBoundL(segment + 1);
}

int EnvelopeShapeGraphic::GetOutputPosition(double output)
{
	return mPaddedRect.T + ((1.0 - output) * (mPaddedRect.B - mPaddedRect.T));
}

void EnvelopeShapeGraphic::InitDimensions()
{
	mPaddedRect = IRECT(GetRECT()->L + kInnerPadPx, GetRECT()->T + kInnerPadPx,
		GetRECT()->R - kInnerPadPx, GetRECT()->B - kInnerPadPx);
	mMaxSegmentWidth = (mPaddedRect.R - mPaddedRect.L) / kNumSegments;

	mTotalWidth = mPaddedRect.R - mPaddedRect.L;
	mTotalHeight = mPaddedRect.B - mPaddedRect.T;
}

void EnvelopeShapeGraphic::InitDividers()
{
	int top = GetRECT()->T;
	int bottom = GetRECT()->B;
	int pad = kDividerWidthPx / 2;
	for (int i = 0; i < kNumDividers; i++) {
		int center = GetSegmentBoundR(i);
		int left = center - pad;
		int right = center + pad;
		mSegmentDividers[i] = IRECT(left, top, right, bottom);
	}
}

void EnvelopeShapeGraphic::DrawRaster(IGraphics* pGraphics)
{
	int left = mPaddedRect.L;
	int top = mPaddedRect.T;
	for (int x = 0; x < mTotalWidth; x++) {
		for (int y = 0; y < mTotalHeight; y++) {
			double brightness = mBrightnessRaster[x][y];
			IColor shadedColor = IColor(0xFF, mColor.R * brightness, mColor.G * brightness, mColor.B * brightness);
			pGraphics->ForcePixel(&shadedColor, left + x, top + y);
		}
	}
}

void EnvelopeShapeGraphic::AllocateRaster()
{
	mBrightnessRaster = new double*[mTotalWidth];
	for (int i = 0; i < mTotalWidth; i++) {
		mBrightnessRaster[i] = new double[mTotalHeight];
	}
}

void EnvelopeShapeGraphic::ZeroRaster()
{
	for (int x = 0; x < mTotalWidth; x++) {
		for (int y = 0; y < mTotalHeight; y++) {
			mBrightnessRaster[x][y];
		}
	}
}

void EnvelopeShapeGraphic::DeleteRaster()
{
	for (int x = 0; x < mTotalWidth; x++) {
		delete[] mBrightnessRaster[x];
	}
	delete[] mBrightnessRaster;
}
*/


/*
void EnvelopeShapeGraphic::MidpointCircle(double** raster, int x0, int y0, int radius)
{
	int x = radius - 1;
	int y = 0;
	int dx = 1;
	int dy = 1;
	int err = dx - (radius << 1);

	while (x >= y) {
		raster[x0 + x][y0 + y] = 1;
		raster[x0 + y][y0 + x] = 1;
		raster[x0 - y][y0 + x] = 1;
		raster[x0 - x][y0 + y] = 1;
		raster[x0 - x][y0 - y] = 1;
		raster[x0 - y][y0 - x] = 1;
		raster[x0 + y][y0 - x] = 1;
		raster[x0 + x][y0 - y] = 1;

		if (err <= 0) {
			y++;
			err += dy;
			dy += 2;
		}
		if (err > 0) {
			x--;
			dx += 2;
			err += dx - (radius << 1);
		}
	}
}

void EnvelopeShapeGraphic::WuDrawLine(double** raster, double x0, double y0, double x1, double y1)
{
	const bool isSteep = abs(y1 - y0) > abs(x1 - x0);
	if (isSteep) {
		std::swap(x0, y0);
		std::swap(x1, y1);
	}
	if (x0 > x1) {
		std::swap(x0, x1);
		std::swap(y0, y1);
	}

	const double dx = x1 - x0;
	const double dy = y1 - y0;
	const double gradient = dx != 0 ? dy / dx : 1.0;

	// First endpoint
	double xend = std::round(x0);
	double yend = y0 + (gradient * (xend - x0));
	double xgap = Wu_rfpart(x0 + 0.5);

	int xpxl1 = (int)xend;
	int ypxl1 = (int)Wu_ipart(yend);
	if (isSteep) {
		Wu_plot(raster, ypxl1, xpxl1, Wu_rfpart(yend) * xgap);
		Wu_plot(raster, ypxl1 + 1, xpxl1, Wu_fpart(yend) * xgap);
	} else {
		Wu_plot(raster, xpxl1, ypxl1, Wu_rfpart(yend) * xgap);
		Wu_plot(raster, xpxl1, ypxl1 + 1, Wu_fpart(yend) * xgap);
	}
	double intery = yend + gradient;

	// Second endpoint
	xend = Wu_round(x1);
	yend = y1 + (gradient * (xend - x1));
	xgap = Wu_fpart(x1 + 0.5);
	int xpxl2 = (int)xend;
	int ypxl2 = (int)Wu_ipart(yend);
	if (isSteep) {
		Wu_plot(raster, ypxl2, xpxl2, Wu_rfpart(yend) * xgap);
		Wu_plot(raster, ypxl2 + 1, xpxl2, Wu_fpart(yend) * xgap);
	} else {
		Wu_plot(raster, xpxl2, ypxl2, Wu_rfpart(yend) * xgap);
		Wu_plot(raster, xpxl2, ypxl2 + 1, Wu_fpart(yend) * xgap);
	}

	// Main loop
	if (isSteep) {
		for (int x = xpxl1 + 1; x <= xpxl2 - 1; x++) {
			Wu_plot(raster, Wu_ipart(intery), x, Wu_rfpart(intery));
			Wu_plot(raster, Wu_ipart(intery) + 1, x, Wu_fpart(intery));
			intery += gradient;
		}
	} else {
		for (int x = xpxl1 + 1; x <= xpxl2 - 1; x++) {
			Wu_plot(raster, x, Wu_ipart(intery), Wu_rfpart(intery));
			Wu_plot(raster, x, Wu_ipart(intery) + 1, Wu_fpart(intery));
			intery += gradient;
		}
	}
}
*/
