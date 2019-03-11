#include "AudioProcessor.h"

AudioProcessor::AudioProcessor(IPlugBase* pPlug, int nSupportedParams)
{
	mpPlug = pPlug;
	mParamIndicies.resize(nSupportedParams);
}

AudioProcessor::~AudioProcessor()
{
}

void AudioProcessor::UpdateParam(int paramIndex)
{
	int paramType = 0;
	int nParams = mParamIndicies.size();
	for (int i = 0; i < nParams; i++)
	{
		if (mParamIndicies[i] = paramIndex)
		{
			paramType = i;
		}
	}

	double newValue = mpPlug->GetParam(paramIndex)->Value();
	int newIntValue = mpPlug->GetParam(paramIndex)->Int();
	HandleParamChange(paramType, newValue, newIntValue);
}

void AudioProcessor::SetSampleRate(double sampleRate)
{
	mSampleRate = sampleRate;
}

void AudioProcessor::Reset()
{
	HandleHostReset();
}

void AudioProcessor::RegisterProcessorParameter(int paramIndex, int paramType)
{
	mParamIndicies[paramType] = paramIndex;
}

void AudioProcessor::HandleParamChange(int paramType, double newValue, int newIntValue)
{
	// By default, do nothing
	// Can be overriden by subclass if desired
}

void AudioProcessor::HandleHostReset()
{
	// By default, do nothing
	// Can be overriden by subclass if desired
}

IParam* AudioProcessor::GetParamObject(int paramType)
{
	return mpPlug->GetParam(mParamIndicies[paramType]);
}

double AudioProcessor::GetSampleRate()
{
	return mSampleRate;
}
