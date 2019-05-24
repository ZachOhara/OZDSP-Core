#include "EnvelopeShapeGraphic.h"

EnvelopeShapeGraphic::EnvelopeShapeGraphic(IPlugBase* pPlug, EnvelopeProcessor* pProcessor, IRECT rect) :
	IControl(pPlug, rect),
	mColor(0xff4caf50),
	mBitmap(rect.W(), rect.H(), kScaleFactor),
	nOutputFrames((rect.W() - (2 * kBoxPadPx)) * kScaleFactor)
{
	mpProcessor = pProcessor;
	AllocateArray();
}
EnvelopeShapeGraphic::~EnvelopeShapeGraphic()
{
	DeleteArrays();
}

bool EnvelopeShapeGraphic::Draw(IGraphics* pGraphics)
{
	CalculateOutputs(mOutputs, nOutputFrames);

	mBitmap.Erase();

	RasterizeOutputs();

	pGraphics->DrawBitmap(&mBitmap.GetIBitmap(), GetRECT());
	
	return true;
}

bool EnvelopeShapeGraphic::IsDirty()
{
	return true;
}

void EnvelopeShapeGraphic::CalculateOutputs(double* outputs, int nFrames)
{
	int maxSegmentWidth = (nFrames - 2) / 4; // -2 because the first and last frames must be zero
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
	int attackRBound = 1 + (attackTime * maxSegmentWidth);
	int decayRBound = attackRBound + (decayTime * maxSegmentWidth);
	int releaseLBound = nFrames - 1 - (releaseTime * maxSegmentWidth);

	// Calculate the data
	outputs[0] = 0;
	CalculateSegmentOutput(outputs, 1, attackRBound, 0.0, 1.0, attackExponent);
	CalculateSegmentOutput(outputs, attackRBound, decayRBound, 1.0, sustainLevel, decayExponent);
	CalculateSegmentOutput(outputs, decayRBound, releaseLBound, sustainLevel, sustainLevel, 1.0);
	CalculateSegmentOutput(outputs, releaseLBound, nFrames - 1, sustainLevel, 0.0, releaseExponent);
	outputs[nFrames - 1] = 0;
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
	int drawWidth = GetRECT()->W() - (2 * kBoxPadPx);
	int drawHeight = GetRECT()->H() - (2 * kBoxPadPx);
	double lastY = kBoxPadPx + ((1 - mOutputs[0]) * drawHeight);
	for (int i = 1; i < nOutputFrames; i++) {
		double progress = ((double)i) / nOutputFrames;
		double x = kBoxPadPx + (progress * drawWidth);
		double y = kBoxPadPx + ((1 - mOutputs[i]) * drawHeight);
		
		DrawThickVerticalLine(x, lastY, y, kLineWidth);
		lastY = y;
	}
}

void EnvelopeShapeGraphic::DrawThickVerticalLine(double x, double y0, double y1, double radius)
{
	if (y0 > y1)
		std::swap(y0, y1);
	for (int y = y0; y <= y1; y++) {
		mBitmap.FillCircle(x, y, radius, mColor);
	}
}

void EnvelopeShapeGraphic::AllocateArray()
{
	mOutputs = new double[nOutputFrames];
}

void EnvelopeShapeGraphic::DeleteArrays()
{
	delete[] mOutputs;
	mOutputs = nullptr;
}
