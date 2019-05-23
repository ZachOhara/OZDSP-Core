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


	static ScaledBitmap scb(mWidthPx, mHeightPx, 16.0);
	scb.Erase();

	// rasterize outputs
	RasterizeOutputs(scb);

	IBitmap& result = scb.GetIBitmap();

	pGraphics->DrawBitmap(&result, &mInnerRect);
	
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

void EnvelopeShapeGraphic::RasterizeOutputs(ScaledBitmap& scb)
{
	int paddedBoundL = kRasterPadPx;
	int paddedBoundR = mWidthPx - kRasterPadPx;

	int outputFloor = GetOutputYPos(0.0) - 1;
	// Draw the left bound
	int firstLeftOutput = GetOutputYPos(mOutputs[paddedBoundL - 1]);
	DrawThickVerticalLine(scb, paddedBoundL - 1, outputFloor,
		firstLeftOutput, kLineRadius);
	// Draw the right bound
	int lastRightOutput = GetOutputYPos(mOutputs[paddedBoundR - 1]);
	int rightOutputFloor = std::max(outputFloor, lastRightOutput);
	// the above line corrects for an artifact that happens when sustain level is zero
	DrawThickVerticalLine(scb, paddedBoundR, lastRightOutput,
		rightOutputFloor, kLineRadius);

	// Draw the rest of the shape
	int lastY = GetOutputYPos(mOutputs[paddedBoundL - 1]);
	for (int x = paddedBoundL; x < paddedBoundR; x++) {
		int y = GetOutputYPos(mOutputs[x]);
		DrawThickVerticalLine(scb, x, lastY, y, kLineRadius);
		lastY = y;
	}
}

void EnvelopeShapeGraphic::DrawThickVerticalLine(ScaledBitmap& scb, int x, int y0, int y1, int radius)
{
	if (y1 < y0)
		std::swap(y0, y1);
	for (int y = y0; y <= y1; y++) {
		scb.FillCircle(x, y, radius, 0xff4caf50);
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
}

void EnvelopeShapeGraphic::DeleteArrays()
{
	delete[] mOutputs;
	mOutputs = nullptr;
}
