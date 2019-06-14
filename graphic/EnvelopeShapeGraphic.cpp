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
	double releaseRbound = 1 - (releaseTime * 0.25);

	if (x < attackRbound) {
		return pow(x / attackRbound, attackExponent);
	} else if (x < decayRbound) {
		return 1 - ((1 - sustainLevel) * pow((x - attackRbound) / (decayRbound - attackRbound), decayExponent));
	} else if (x < releaseRbound) {
		return sustainLevel;
	} else {
		return sustainLevel - (sustainLevel * pow((x - releaseRbound) / (1 - releaseRbound), releaseExponent));
	}
}

double EnvelopeShapeGraphic::GetThickFunctionTop(double x, double halfweight)
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
	double releaseRbound = 1 - (releaseTime * 0.25);

	if (x < attackRbound) {
		return AttackOutput(x + halfweight) + halfweight;
	}
	else if (x < decayRbound) {

		return DecayOutput(x - halfweight) + halfweight;
	}
	else if (x < releaseRbound) {
		return sustainLevel + (2 * halfweight);
	}
	else {
		return ReleaseOutput(x - halfweight) + halfweight;
	}

	/*
	if (x < attackRbound)
		return GetFunctionValue(x + halfweight) + halfweight;
	else
		return GetFunctionValue(x - halfweight) + halfweight;
	*/
}

double EnvelopeShapeGraphic::GetThickFunctionBottom(double x, double halfweight)
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
	double releaseRbound = 1 - (releaseTime * 0.25);

	if (x < attackRbound) {
		return AttackOutput(x - halfweight) - halfweight;
	}
	else if (x < decayRbound) {

		return DecayOutput(x + halfweight) - halfweight;
	}
	else if (x < releaseRbound) {
		return sustainLevel - (2*halfweight);
	}
	else {
		return ReleaseOutput(x + halfweight) - halfweight;
	}

	/*
	if (x < attackRbound)
		return GetFunctionValue(x - halfweight) - halfweight;
	else
		return GetFunctionValue(x + halfweight) - halfweight;
	*/
}

double EnvelopeShapeGraphic::AttackOutput(double x)
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
	double releaseRbound = 1 - (releaseTime * 0.25);

	return pow(x / attackRbound, attackExponent);
}

double EnvelopeShapeGraphic::DecayOutput(double x)
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
	double releaseRbound = 1 - (releaseTime * 0.25);

	return 1 - ((1 - sustainLevel) * pow((x - attackRbound) / (decayRbound - attackRbound), decayExponent));
}

double EnvelopeShapeGraphic::SustainOutput(double x)
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
	double releaseRbound = 1 - (releaseTime * 0.25);

	
	return sustainLevel;
}

double EnvelopeShapeGraphic::ReleaseOutput(double x)
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
	double releaseRbound = 1 - (releaseTime * 0.25);

	return sustainLevel - (sustainLevel * pow((x - releaseRbound) / (1 - releaseRbound), releaseExponent));
}

