#include "CommonPlugBase.h"

CommonPlugBase::CommonPlugBase(
	IPlugInstanceInfo instanceInfo,
	int nParams,
	int nPresets,
	IGraphics* pGraphics,
	const char* channelIOStr,
	const char* effectName,
	const char* mfrName,
	int vendorVersion,
	int uniqueID,
	int mfrID,
	int latency,
	bool plugDoesMidi,
	bool plugDoesChunks,
	bool plugIsInst,
	int plugAPI) :
	IPlug(
		instanceInfo,
		nParams,
		channelIOStr,
		nPresets,
		effectName,
		"",
		mfrName,
		vendorVersion,
		uniqueID,
		mfrID,
		latency,
		plugDoesMidi,
		plugDoesChunks,
		plugIsInst,
		plugAPI
	)
{
	TRACE;
	mpGraphics = pGraphics;
}

CommonPlugBase::~CommonPlugBase()
{
}

void CommonPlugBase::Reset()
{
	TRACE;
	IMutexLock lock(this);
}

void CommonPlugBase::OnParamChange(int paramIdx)
{
	IMutexLock lock(this);
}

void CommonPlugBase::CreatePresets()
{

}

IGraphics* CommonPlugBase::GetGraphics()
{
	return mpGraphics;
}

void CommonPlugBase::InitializeParameter(ParameterInfo& param, IBitmap& bitmap)
{
	if (param.IsParam())
	{
		IParam* pParamObj = GetParam(param.ParamIndex());

		// Initialize selection parameter
		if (param.IsParamSelection())
		{
			// Initialize the enum
			pParamObj->InitEnum(
				param.Name().c_str(), 0, param.NumStates(), "");
			// Populate the state list
			std::vector<std::string>& stateList = param.StateList();
			for (int i = 0; i < param.NumStates(); i++)
			{
				pParamObj->SetDisplayText(i, stateList[i].c_str());
			}
			// Initialize graphics
			if (param.IsSelectionDropdown())
			{
				// TODO: dropdown
			}
			else
			{
				GetGraphics()->AttachControl(new ISwitchControl(
					this, param.PosX(), param.PosY(), param.ParamIndex(), &bitmap));
			}
		}

		// Initialize numeric parameter
		if (param.IsParamNumeric())
		{
			// Initialize the double
			pParamObj->InitDouble(
				param.Name().c_str(), param.DefaultValue(), param.MinValue(),
				param.MaxValue(), param.ValueStep(), param.UnitLabel().c_str());
			pParamObj->SetShape(
				param.ValueShapeFactor());

			// Initialize graphics
			GetGraphics()->AttachControl(new IKnobMultiControl(
				this, param.PosX(), param.PosY(), param.ParamIndex(), &bitmap));
		}
		// TODO: if param has a label, do stuff here
	}
}
