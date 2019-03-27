#include "ModularProcessor.h"

ModularProcessor::ModularProcessor(int nParams)
{
	mSampleRate = 1.0; // prevents division by zero errors in subclasses
	for (int i = 0; i < nParams; i++) {
		mParamIndicies.push_back(-1);
	}
}

ModularProcessor::~ModularProcessor()
{
}

void ModularProcessor::NotifyParamChange(int paramIndex)
{
	int paramType = LookupParamType(paramIndex);
	if (paramType != -1) {
		double newValue = mpPlug->GetParam(paramIndex)->Value();
		int newIntValue = mpPlug->GetParam(paramIndex)->Int();
		HandleParamChange(paramType, newValue, newIntValue);
	}
}

void ModularProcessor::SetSampleRate(double sampleRate)
{
	mSampleRate = sampleRate;
}

void ModularProcessor::Reset()
{
	HandleHostReset();
}

void ModularProcessor::RegisterParameterList(std::vector<std::pair<int, int>> parameterList)
{
	int nParams = parameterList.size();
	for (int i = 0; i < nParams; i++) {
		RegisterParameter(parameterList[i].first, parameterList[i].second);
	}
}

void ModularProcessor::RegisterParameter(int paramIndex, int paramType)
{
	mParamIndicies[paramType] = paramIndex;
}

void ModularProcessor::SetParentPlugin(IPlugBase* pPlug)
{
	mpPlug = pPlug;
}

double ModularProcessor::GetParamValue(int paramType)
{
	return GetParamObject(paramType)->Value();
}

int ModularProcessor::GetParamIntValue(int paramType)
{
	return GetParamObject(paramType)->Int();
}

double ModularProcessor::GetParamNormalized(int paramType)
{
	return GetParamObject(paramType)->GetNormalized();
}

void ModularProcessor::HandleParamChange(int paramType, double newValue, int newIntValue)
{
	// By default, do nothing
	// Can be overriden by subclass if desired
}

void ModularProcessor::HandleHostReset()
{
	// By default, do nothing
	// Can be overriden by subclass if desired
}

IParam* ModularProcessor::GetParamObject(int paramType)
{
	return mpPlug->GetParam(mParamIndicies[paramType]);
}

double ModularProcessor::GetSampleRate()
{
	return mSampleRate;
}

int ModularProcessor::LookupParamType(int paramIndex)
{
	int nParams = mParamIndicies.size();
	for (int i = 0; i < nParams; i++) {
		if (mParamIndicies[i] == paramIndex)
			return i;
	}
	return -1;
}
