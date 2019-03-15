#ifndef __MODULAR_PROCESSOR__H__
#define __MODULAR_PROCESSOR__H__

#include "IPlug_include_in_plug_hdr.h"

#include <vector>

class ModularProcessor
{
public:
	ModularProcessor(int nParams);
	~ModularProcessor();

	void NotifyParamChange(int paramIndex);
	void SetSampleRate(double sampleRate);
	void Reset();

	void RegisterParameterList(std::vector<std::pair<int, int>> parameterList);
	void RegisterParameter(int paramIndex, int paramType);
	void SetParentPlugin(IPlugBase* pPlug);

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

#endif // !__MODULAR_PROCESSOR__H__
