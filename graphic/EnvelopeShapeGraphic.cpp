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
}

void EnvelopeShapeGraphic::DeleteArrays()
{
	delete[] mOutputs;
	mOutputs = nullptr;
	delete[] mRaster;
	mRaster = nullptr;
}

inline int EnvelopeShapeGraphic::Index(int x, int y)
{
	return (x * mHeightPx) + y;
}
