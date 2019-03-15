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

	// Reset processors
	double sampleRate = GetSampleRate();
	int nProcessors = mProcessorRegistry.size();
	for (int i = 0; i < nProcessors; i++) {
		mProcessorRegistry[i]->SetSampleRate(sampleRate);
		mProcessorRegistry[i]->Reset();
	}
}

void CommonPlugBase::OnParamChange(int paramIndex)
{
	IMutexLock lock(this);

	// Notify the processors
	int nProcessors = mProcessorRegistry.size();
	for (int i = 0; i < nProcessors; i++) {
		mProcessorRegistry[i]->NotifyParamChange(paramIndex);
	}

	// Update displays
	auto search = mLabelRegistry.find(paramIndex);
	if (search != mLabelRegistry.end()) {
		ParameterValueLabel* label = search->second;
		label->UpdateDisplay();
	}
}

void CommonPlugBase::CreatePresets()
{
	// By default, do nothing
	// This method should be overriden when presets are implemented
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

void CommonPlugBase::AddParameterList(std::vector<ParameterInfo>& parameterList)
{
	int nParams = parameterList.size();
	for (int i = 0; i < nParams; i++) {
		AddParameter(parameterList[i]);
	}
}

void CommonPlugBase::AddParameter(ParameterInfo& param)
{
	// First check to make sure the parameter is initialied
	if (param.IsParam()) {
		IParam* pParamObj = GetParam(param.ParamIndex());
		IBitmap& bitmap = mBitmapRegistry.find(param.BitmapId())->second;
		if (param.IsParamSelection())
			AddSelectionParameter(param, pParamObj, bitmap);
		if (param.IsParamNumeric())
			AddNumericParameter(param, pParamObj, bitmap);
		if (param.IsParamLabeled())
			AddParameterLabel(param, pParamObj, bitmap);
	}
}

void CommonPlugBase::RegisterProcessorList(std::vector<ModularProcessor*> processorList)
{
	int nProcessors = processorList.size();
	for (int i = 0; i < nProcessors; i++) {
		RegisterProcessor(processorList[i]);
	}
}

void CommonPlugBase::RegisterProcessor(ModularProcessor* processor)
{
	processor->SetParentPlugin(this);
	mProcessorRegistry.push_back(processor);
}

void CommonPlugBase::FinishConstruction()
{
	AttachGraphics(GetGraphics());
	CreatePresets();
	ForceUpdateParameters();
}

void CommonPlugBase::AddSelectionParameter(ParameterInfo& param, IParam* pParamObj, IBitmap& bitmap)
{
	// Initialize the enum
	pParamObj->InitEnum(
		param.Name().c_str(), 0, param.NumStates(), "");

	// Populate the state list
	std::vector<std::string>& stateList = param.StateList();
	for (int i = 0; i < param.NumStates(); i++) {
		pParamObj->SetDisplayText(i, stateList[i].c_str());
	}

	// Initialize graphics
	if (param.IsSelectionDropdown()) {
		// TODO: dropdown
	} else {
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
	int nSpecialValues = specialValues.size();
	for (int i = 0; i < nSpecialValues; i++) {
		int value = specialValues[i].first;
		std::string& display = specialValues[i].second;
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

void CommonPlugBase::ForceUpdateParameters()
{
	int nParams = NParams();
	for (int i = 0; i < nParams; i++) {
		OnParamChange(i);
	}
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
