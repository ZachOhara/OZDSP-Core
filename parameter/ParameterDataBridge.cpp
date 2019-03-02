#include "ParameterDataBridge.h"

ParameterDataBridge::ParameterDataBridge(IParam* pParam)
{
	mpParam = pParam;
}

ParameterDataBridge::~ParameterDataBridge()
{
}

IParam* ParameterDataBridge::GetParam()
{
	return mpParam;
}

double ParameterDataBridge::ParamValue()
{
	return mpParam->Value();
}

int ParameterDataBridge::ParamInt()
{
	return mpParam->Int();
}

bool ParameterDataBridge::IsParamMinimized()
{
	return (mpParam->Value() == mpParam->GetMin());
}

bool ParameterDataBridge::IsParamMaximized()
{
	return (mpParam->Value() == mpParam->GetMax());
}

OscillatorFrequencyDataBridge::OscillatorFrequencyDataBridge(IParam* pParam, Oscillator* pProcessor) :
	ParameterDataBridge(pParam)
{
	mpProcessor = pProcessor;
}

OscillatorFrequencyDataBridge::~OscillatorFrequencyDataBridge()
{
}

void OscillatorFrequencyDataBridge::HandleUpdate()
{
	mpProcessor->SetFrequency(ParamValue());
}

OscillatorWaveformDataBridge::OscillatorWaveformDataBridge(IParam* pParam, Oscillator* pProcessor) :
	ParameterDataBridge(pParam)
{
	mpProcessor = pProcessor;
}

OscillatorWaveformDataBridge::~OscillatorWaveformDataBridge()
{
}

void OscillatorWaveformDataBridge::HandleUpdate()
{
	mpProcessor->SetMode(ParamInt());
}

VolumeDataBridge::VolumeDataBridge(IParam* pParam, VolumeProcessor* pProcessor) :
	ParameterDataBridge(pParam)
{
	mpProcessor = pProcessor;
}

VolumeDataBridge::~VolumeDataBridge()
{
}

void VolumeDataBridge::HandleUpdate()
{
	// If sufficiently low, just set to the minimum
	// This ensures proper display of "-oo dB" at min value
	static const double threshold_db = -99.95;
	if (ParamValue() <= threshold_db && !IsParamMinimized())
	{
		GetParam()->Set(GetParam()->GetMin());
	}
	// Now update the processor
	if (IsParamMinimized())
	{
		mpProcessor->SetZero();
	}
	else
	{
		mpProcessor->SetDecibels(ParamValue());
	}
}
