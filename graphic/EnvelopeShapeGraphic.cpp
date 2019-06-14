#include "EnvelopeShapeGraphic.h"

EnvelopeShapeGraphic::EnvelopeShapeGraphic(IPlugBase* pPlug, EnvelopeProcessor* pProcessor, IRECT rect) :
	FunctionLineGraphic(pPlug, rect, 0xffffffff)
{
	mpProcessor = pProcessor;
}
EnvelopeShapeGraphic::~EnvelopeShapeGraphic()
{
}

bool EnvelopeShapeGraphic::IsDirty()
{
	return true;
}

double EnvelopeShapeGraphic::GetFunctionValue(double x)
{
	if (x < 0.25)
		return pow(x / 0.25, 2);
	else if (x < 0.5)
		return 1 - (0.5 * pow((x - 0.25) / 0.25, 2));
	else if (x < 0.75)
		return 0.5;
	else
		return 0.5 - (0.5 * pow((x - 0.75) / 0.25, 2));
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
