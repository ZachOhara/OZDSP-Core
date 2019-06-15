#include "EnvelopeShapeGraphic.h"

EnvelopeShapeGraphic::EnvelopeShapeGraphic(IPlugBase* pPlug, IRECT rect, EnvelopeProcessor* pProcessor) :
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

double EnvelopeShapeGraphic::GetFunctionValue(double x, double ymax)
{
	// TODO cache these variables as soon as they are set
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

	double attackRbound = attackTime * 0.25;
	double decayRbound = attackRbound + (decayTime * 0.25);
	double releaseLbound = 1 - (releaseTime * 0.25);

	if (x < attackRbound) {
		double seg_progress = x / attackRbound;
		return ymax * pow(seg_progress, attackExponent);
	} else if (x < decayRbound) {
		double seg_progress = (x - attackRbound) / (decayRbound - attackRbound);
		return ymax * (1 - ((1 - sustainLevel) * pow(seg_progress, decayExponent)));
	} else if (x < releaseLbound) {
		return ymax * sustainLevel;
	} else {
		double seg_progress = (x - releaseLbound) / (1 - releaseLbound);
		return ymax * (sustainLevel - (sustainLevel * pow(seg_progress, releaseExponent)));
	}
}

double EnvelopeShapeGraphic::GetLocalDerivative(double x, double ymax)
{
	// TODO cache these variables as soon as they are set
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

	double attackRbound = attackTime * 0.25;
	double decayRbound = attackRbound + (decayTime * 0.25);
	double releaseLbound = 1 - (releaseTime * 0.25);

	if (x < attackRbound) {
		double seg_progress = x / attackRbound;
		return attackExponent * pow(seg_progress, attackExponent - 1);
	}
	else if (x < decayRbound) {
		double seg_progress = (x - attackRbound) / (decayRbound - attackRbound);
		return -0.5 * (1 - sustainLevel) * (decayExponent * pow(seg_progress, decayExponent - 1));
	}
	else if (x < releaseLbound) {
		return 0;
	}
	else {
		double seg_progress = (x - releaseLbound) / (1 - releaseLbound);
		return -0.5 * sustainLevel * (releaseExponent * pow(seg_progress, releaseExponent - 1));
	}
}
