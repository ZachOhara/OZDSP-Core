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

void CommonPlugBase::SetBackground(int id, std::string name)
{
	GetGraphics()->AttachBackground(id, name.c_str());
}

void CommonPlugBase::RegisterBitmap(int id, std::string name, int nFrames)
{
	IBitmap& bitmap = GetGraphics()->LoadIBitmap(id, name.c_str(), nFrames);
	mBitmapRegistry[id] = bitmap;
}

void CommonPlugBase::ForceUpdateParameters()
{
	int nParams = NParams();
	for (int i = 0; i < nParams; i++)
	{
		OnParamChange(i);
	}
}

void CommonPlugBase::FinishConstruction()
{
	AttachGraphics(GetGraphics());
	CreatePresets();
	ForceUpdateParameters();
}

void CommonPlugBase::AddParameters(std::vector<ParameterInfo>& paramList)
{
	int nParams = paramList.size();
	for (int i = 0; i < nParams; i++)
	{
		AddParameter(paramList[i]);
	}
}

void CommonPlugBase::AddParameter(ParameterInfo& param)
{

	if (param.IsParam())
	{
		IParam* pParamObj = GetParam(param.ParamIndex());
		IBitmap& bitmap = mBitmapRegistry.find(param.BitmapId())->second;

		if (param.IsParamSelection())
		{
			AddSelectionParameter(param, pParamObj, bitmap);
		}

		if (param.IsParamNumeric())
		{
			AddNumericParameter(param, pParamObj, bitmap);
		}

		// TODO: if param has a label, do stuff here
	}
}

void CommonPlugBase::AddSelectionParameter(ParameterInfo& param, IParam* pParamObj, IBitmap& bitmap)
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

void CommonPlugBase::AddNumericParameter(ParameterInfo& param, IParam* pParamObj, IBitmap& bitmap)
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

void CommonPlugBase::AddParameterLabel(ParameterInfo& param, IParam* pParamObj, IBitmap& bitmap)
{
	// big TODO
}
