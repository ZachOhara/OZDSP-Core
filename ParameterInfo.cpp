#include "ParameterInfo.h"

ParameterInfo::ParameterInfo()
{
}

ParameterInfo::~ParameterInfo()
{
}

ParameterInfo& ParameterInfo::MakeFrequencyParam()
{
	static const double min_hz = 20.0;
	static const double max_hz = 15000.0;
	static const double default_hz = 440.0; // A 4
	static const double step_hz = 0.01;
	return (*this)
		.InitNumericParam(default_hz, min_hz, max_hz, step_hz, "Hz")
		.SetValueShapeFactor(3.0);
}

ParameterInfo& ParameterInfo::MakeVolumeReductionParam()
{
	static const double min_db = -100.0;
	static const double max_db = 0.0;
	static const double default_db = 0.0;
	static const double step_db = 0.1;
	return (*this)
		.InitNumericParam(default_db, min_db, max_db, step_db, "dB")
		.SetValueShapeFactor(0.2);
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
