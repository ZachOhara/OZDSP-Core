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

void CommonPlugBase::OnParamChange(int paramIndex)
{
	IMutexLock lock(this);

	// Update data bridges
	int nBridges = mDataBridgeList.size();
	for (int i = 0; i < nBridges; i++)
	{
		if (paramIndex == mDataBridgeList[i].first)
		{
			mDataBridgeList[i].second->HandleUpdate();
		}
	}

	// Update displays
	auto search = mLabelRegistry.find(paramIndex);
	if (search != mLabelRegistry.end())
	{
		ParameterValueLabel* label = search->second;
		label->UpdateDisplay();
	}
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

void CommonPlugBase::AddOscillatorFrequencyBridge(int paramIndex, Oscillator* pProcessor)
{
	ParameterDataBridge* bridge = new OscillatorFrequencyDataBridge(GetParam(paramIndex), pProcessor);
	AddParameterBridge(paramIndex, bridge);
}

void CommonPlugBase::AddOscillatorWaveformBridge(int paramIndex, Oscillator* pProcessor)
{
	ParameterDataBridge* bridge = new OscillatorWaveformDataBridge(GetParam(paramIndex), pProcessor);
	AddParameterBridge(paramIndex, bridge);
}

void CommonPlugBase::AddToneParamBridge(int paramIndex, ToneProcessor* pProcessor)
{
	ParameterDataBridge* bridge = new ToneDataBridge(GetParam(paramIndex), pProcessor);
	AddParameterBridge(paramIndex, bridge);
}

void CommonPlugBase::AddVolumeParamBridge(int paramIndex, VolumeProcessor* pProcessor)
{
	ParameterDataBridge* bridge = new VolumeDataBridge(GetParam(paramIndex), pProcessor);
	AddParameterBridge(paramIndex, bridge);
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

		if (param.IsParamLabeled())
		{
			AddParameterLabel(param, pParamObj, bitmap);
		}
	}
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

void CommonPlugBase::AddParameterBridge(int paramIndex, ParameterDataBridge* pBridge)
{
	mDataBridgeList.push_back(std::pair<int, ParameterDataBridge*>(paramIndex, pBridge));
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

	// Add special values
	std::vector<std::pair<int, std::string>>& specialValues = param.SpecialDisplayValues();
	for (auto iterator = specialValues.begin(); iterator != specialValues.end(); ++iterator)
	{
		int value = iterator->first;
		std::string& display = iterator->second;
		pParamObj->SetDisplayText(value, display.c_str());
	}

	// Initialize graphics
	GetGraphics()->AttachControl(new IKnobMultiControl(
		this, param.PosX(), param.PosY(), param.ParamIndex(), &bitmap));
}

void CommonPlugBase::AddParameterLabel(ParameterInfo& param, IParam* pParamObj, IBitmap& bitmap)
{
	IRECT labelRect = ConstructLabelRect(param, bitmap);
	IRECT editingRect = ConstructEditingRect(param, labelRect);
	ParameterValueLabel* label = new ParameterValueLabel(this, param.ParamIndex(),
		labelRect, editingRect, param.LabelFontSize());
	label->SetTextEntryLength(param.LabelEditChars());
	mLabelRegistry[param.ParamIndex()] = label;
	GetGraphics()->AttachControl(label);
}

IRECT CommonPlugBase::ConstructLabelRect(ParameterInfo& param, IBitmap& bitmap)
{
	int controlX = param.PosX();
	int controlY = param.PosY();
	int controlWidth = bitmap.frameWidth();
	int controlHeight = bitmap.frameHeight();

	int sideBuffer = param.LabelWidthBuffer() / 2;

	int rectLeft = controlX - sideBuffer;
	int rectRight = controlX + controlWidth + sideBuffer;

	int rectTop = controlY + controlHeight + param.LabelVerticalBuffer();
	int rectBottom = rectTop + param.LabelHeight();

	return IRECT(rectLeft, rectTop, rectRight, rectBottom);
}

IRECT CommonPlugBase::ConstructEditingRect(ParameterInfo& param, IRECT& labelRect)
{
	int width = labelRect.R - labelRect.L;
	int sideDifference = (width - param.LabelEditWidth()) / 2;
	return IRECT(labelRect.L + sideDifference, labelRect.T,
		labelRect.R - sideDifference, labelRect.B);
}
