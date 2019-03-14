#ifndef __AUDIO_PROCESSOR__H__
#define __AUDIO_PROCESSOR__H__

#include "IPlug_include_in_plug_hdr.h"

#include <vector>

class ModularProcessor
{
public:
	ModularProcessor(IPlugBase* pPlug, int nParams);
	~ModularProcessor();

	void NotifyParamChange(int paramIndex);
	void SetSampleRate(double sampleRate);
	void Reset();

	void RegisterParameter(int paramIndex, int paramType);

protected:
	virtual void HandleParamChange(int paramType, double newValue, int newIntValue);
	virtual void HandleHostReset();

	IParam* GetParamObject(int paramType);
	double GetSampleRate();

private:
	IPlugBase* mpPlug;

	std::vector<int> mParamIndicies;

	double mSampleRate;

	int LookupParamType(int paramIndex);
};

#endif // !__AUDIO_PROCESSOR__H__
