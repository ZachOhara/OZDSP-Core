#ifndef __PARAMETER_DATA_BRIDGE_H__
#define __PARAMETER_DATA_BRIDGE_H__

#include "IPlug_include_in_plug_hdr.h"

#include "../processing/Oscillator.h"
#include "../processing/ToneProcessor.h"
#include "../processing/VolumeProcessor.h"

class ParameterDataBridge
{
public:
	ParameterDataBridge(IParam* pParam);
	~ParameterDataBridge();

	virtual void HandleUpdate() = 0;

protected:
	IParam* GetParam();

	double ParamValue();
	int ParamInt();

	bool IsParamMinimized();
	bool IsParamMaximized();

private:
	IParam* mpParam;
};

class OscillatorFrequencyDataBridge : public ParameterDataBridge
{
public:
	OscillatorFrequencyDataBridge(IParam* pParam, Oscillator* pProcessor);
	~OscillatorFrequencyDataBridge();

	void HandleUpdate() override;

private:
	Oscillator* mpProcessor;
};

class OscillatorWaveformDataBridge : public ParameterDataBridge
{
public:
	OscillatorWaveformDataBridge(IParam* pParam, Oscillator* pProcessor);
	~OscillatorWaveformDataBridge();

	void HandleUpdate() override;

private:
	Oscillator* mpProcessor;
};

class VolumeDataBridge : public ParameterDataBridge
{
public:
	VolumeDataBridge(IParam* pParam, VolumeProcessor* pProcessor);
	~VolumeDataBridge();

	void HandleUpdate() override;

private:
	VolumeProcessor* mpProcessor;
};

#endif // !__PARAMETER_DATA_BRIDGE_H__
