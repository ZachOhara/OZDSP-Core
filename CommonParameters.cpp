#include "CommonParameters.h"

void InitVolumeParameter(IParam* pParam)
{
	const double min_db = -100.0;
	const double max_db = 0.0;
	const double default_db = 0.0;

	pParam->InitDouble("Volume", default_db, min_db, max_db, 0.1, "dB");
	pParam->SetShape(0.2); // determined experimentally
	pParam->SetDisplayText(-100, "-oo dB");
}

void MinimizeVolumeParam(IParam* pParam)
{
	const double threshold_db = -99.95;
	if (pParam->Value() <= threshold_db)
	{
		// First check if it's already at min, otherwise we'll get infinite recursion
		if (pParam->Value() != pParam->GetMin())
		{
			pParam->Set(pParam->GetMin());
		}
	}
}

void HandleVolumeParamChange(IParam* pParam, VolumeControl* pVolControl)
{
	if (IsParamMinimized(pParam))
	{
		pVolControl->SetZero();
	}
	else
	{
		pVolControl->SetDecibels(pParam->Value());
	}
}

void InitFrequencyParameter(IParam* pParam)
{
	const double min_hz = 40.0;
	const double max_hz = 15000.0;
	const double default_hz = 440.0; // A 4
	pParam->InitDouble("Frequency", default_hz, min_hz, max_hz, 0.01, "Hz");
	pParam->SetShape(3.0);
}

void InitFrequencyLabel(ParamValueLabel* pLabel)
{
	pLabel->SetTextEntryWidth(80);
	pLabel->SetTextEntryLength(9);
}

void InitWaveformParameter(IParam* pParam)
{
	pParam->InitEnum("Waveform", 0, kNumOscillatorModes, "");
	pParam->SetDisplayText(0, "Sine");
	pParam->SetDisplayText(1, "Triangle");
	pParam->SetDisplayText(2, "Square");
	pParam->SetDisplayText(3, "Sawtooth");
}

void ForceUpdateParams(IPlugBase* pPlug)
{
	int nParams = pPlug->NParams();
	for (int i = 0; i < nParams; i++)
	{
		pPlug->OnParamChange(i);
	}
}

bool IsParamMinimized(IParam* pParam)
{
	return (pParam->Value() == pParam->GetMin());
}
