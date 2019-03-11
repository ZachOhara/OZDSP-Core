#ifndef __AUDIO_PROCESSOR__H__
#define __AUDIO_PROCESSOR__H__

#include "IPlug_include_in_plug_hdr.h"

#include <vector>

class AudioProcessor
{
public:
	AudioProcessor(IPlugBase* pPlug, int nSupportedParams);
	~AudioProcessor();

	void UpdateParam(int paramIndex);
	void SetSampleRate(double sampleRate);
	void Reset();

	void RegisterProcessorParameter(int paramIndex, int paramType);

protected:
	virtual void HandleParamChange(int paramType, double newValue, int newIntValue);
	virtual void HandleHostReset();

	IParam* GetParamObject(int paramType);
	double GetSampleRate();

private:
	IPlugBase* mpPlug;

	std::vector<int> mParamIndicies;

	double mSampleRate;
};

#endif // !__AUDIO_PROCESSOR__H__
