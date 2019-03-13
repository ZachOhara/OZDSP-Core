#include "ParameterInfo.h"

ParameterInfo::ParameterInfo()
{
}

ParameterInfo::~ParameterInfo()
{
}

ParameterInfo & ParameterInfo::MakeEnvelopeTimeParam()
{
	static const double min_s = 0.001;
	static const double max_s = 3.0;
	static const double default_s = 0.001;
	static const double step_s = 0.001;
	return (*this)
		.InitNumericParam(default_s, min_s, max_s, step_s, "s");
}

ParameterInfo & ParameterInfo::MakeEnvelopeShapeParam()
{
	return (*this).InitSelectionParam({ "Linear", "Exponential", "Logarithmic" }, false);
}

ParameterInfo& ParameterInfo::MakeFrequencyParam()
{
	static const double min_hz = 20.0;
	static const double max_hz = 15000.0;
	static const double default_hz = 440.0; // A 4
	static const double step_hz = 0.01;
	return (*this)
		.InitNumericParam(default_hz, min_hz, max_hz, step_hz, "Hz")
		.SetValueShapeFactor(3.0)
		.SetLabelEditWidth(80)
		.SetLabelEditChars(8);
}

ParameterInfo& ParameterInfo::MakePercentageParam(double defaultValue)
{
	return (*this)
		.InitNumericParam(defaultValue, 0.0, 100.0, 0.01, "%");
}

ParameterInfo& ParameterInfo::MakeVolumeReductionParam()
{
	static const double min_db = -100.0;
	static const double max_db = 0.0;
	static const double default_db = 0.0;
	static const double step_db = 0.1;
	static const double shapeFactor = 0.137808; // see note at bottom of this function
	return (*this)
		.InitNumericParam(default_db, min_db, max_db, step_db, "dB")
		.SetValueShapeFactor(shapeFactor)
		.AddSpecialDisplayValue(-100, "-oo dB");
	/*
	 * I could write a paper on the value of shapeFactor and why it's perfectly tuned
	 * but you'll just have to trust me on it. It's set based on the way humans
	 * percieve loudness of sounds relative to amplitude, and also on the relationship
	 * between amplitude and decibels. This particular value is the shape that makes
	 * the loudness of a decibel knob seem linear (e.g. if the knob is turned halfway,
	 * the resulting signal should *sound* half as loud). Note that this *is* affected
	 * by whatever the minimum decibel value is.
	 */
}

ParameterInfo& ParameterInfo::MakeWaveformParam()
{
	return (*this)
		.InitSelectionParam({ "Sine", "Triangle", "Square", "Sawtooth" }, false);
}

ParameterInfo& ParameterInfo::InitParam(std::string name, int paramIdx, int bitmapId, int posX, int posY)
{
	mIsParam = true;
	mParamName = name;
	mParamIndex = paramIdx;
	mBitmapId = bitmapId;
	mPosX = posX;
	mPosY = posY;
	return *this;
}

ParameterInfo& ParameterInfo::InitSelectionParam(std::vector<std::string> states, bool isDropdown)
{
	mIsParamSelection = true;
	mStateList = std::vector<std::string>(states);
	mIsDropdown = isDropdown;
	return *this;
}

ParameterInfo& ParameterInfo::InitNumericParam(double defaultValue, double min, double max, double step, std::string unit)
{
	mIsParamNumeric = true;
	mDefaultValue = defaultValue;
	mMinValue = min;
	mMaxValue = max;
	mValueStep = step;
	mUnitLabel = std::string(unit);	
	return *this;
}

ParameterInfo& ParameterInfo::InitLabel()
{
	mIsParamLabeled = true;
	// Use the pre-set default values
	return *this;
}

ParameterInfo& ParameterInfo::SetValueShapeFactor(double weightFactor)
{
	mValueShapeFactor = weightFactor;
	return *this;
}

ParameterInfo & ParameterInfo::AddSpecialDisplayValue(int value, std::string display)
{
	mSpecialDisplayValues.push_back(std::pair<int, std::string>(value, display));
	return *this;
}

ParameterInfo& ParameterInfo::SetLabelEditWidth(int editWidth)
{
	mLabelEditWidth = editWidth;
	return *this;
}

ParameterInfo& ParameterInfo::SetLabelEditChars(int editChars)
{
	mLabelEditChars = editChars;
	return *this;
}

bool ParameterInfo::IsParam()
{
	return mIsParam;
}

std::string& ParameterInfo::Name()
{
	return mParamName;
}

int ParameterInfo::ParamIndex()
{
	return mParamIndex;
}

int ParameterInfo::BitmapId()
{
	return mBitmapId;
}

int ParameterInfo::PosX()
{
	return mPosX;
}

int ParameterInfo::PosY()
{
	return mPosY;
}

bool ParameterInfo::IsParamSelection()
{
	return mIsParamSelection;
}

int ParameterInfo::NumStates()
{
	return mStateList.size();
}

std::vector<std::string>& ParameterInfo::StateList()
{
	return mStateList;
}

bool ParameterInfo::IsSelectionDropdown()
{
	return mIsDropdown;
}

bool ParameterInfo::IsParamNumeric()
{
	return mIsParamNumeric;
}

double ParameterInfo::DefaultValue()
{
	return mDefaultValue;
}

double ParameterInfo::MinValue()
{
	return mMinValue;
}

double ParameterInfo::MaxValue()
{
	return mMaxValue;
}

double ParameterInfo::ValueStep()
{
	return mValueStep;
}

std::string& ParameterInfo::UnitLabel()
{
	return mUnitLabel;
}

double ParameterInfo::ValueShapeFactor()
{
	return mValueShapeFactor;
}

std::vector<std::pair<int, std::string>>& ParameterInfo::SpecialDisplayValues()
{
	return mSpecialDisplayValues;
}

bool ParameterInfo::IsParamLabeled()
{
	return mIsParamLabeled;
}

int ParameterInfo::LabelVerticalBuffer()
{
	return mLabelVerticalBuffer;
}

int ParameterInfo::LabelWidthBuffer()
{
	return mLabelWidthBuffer;
}

int ParameterInfo::LabelHeight()
{
	return mLabelHeight;
}

int ParameterInfo::LabelEditWidth()
{
	return mLabelEditWidth;
}

int ParameterInfo::LabelEditChars()
{
	return mLabelEditChars;
}

int ParameterInfo::LabelFontSize()
{
	return mLabelFontSize;
}
