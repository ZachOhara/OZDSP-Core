#ifndef  __PARAMETER_INFO__
#define __PARAMETER_INFO__

#include "IPlug_include_in_plug_hdr.h"

#include <string>
#include <vector>

/*
enum ECommonParameters
{
	kUnspecified,
	kFrequency,
	kToneMix,
	kVolume,
	kWaveformSelect,
};
*/

class ParameterInfo
{
public:
	ParameterInfo();
	~ParameterInfo();

	// Common parameter types
	ParameterInfo& MakeFrequencyParam();
	ParameterInfo& MakeVolumeReductionParam();
	ParameterInfo& MakeWaveformParam();

	// General initializers
	ParameterInfo& InitParam(std::string name, int paramIdx, int posX, int posY);
	ParameterInfo& InitSelectionParam(std::vector<std::string> states, bool isDropdown);
	ParameterInfo& InitNumericParam(double defaultValue, double min, double max,
		double step, std::string unit);
	ParameterInfo& InitLabel();

	// Setters for specific settings
	ParameterInfo& SetValueShapeFactor(double weightFactor);

	// Accessors for all parameter types
	bool IsParam();
	std::string& Name();
	int ParamIndex();
	int PosX();
	int PosY();

	// Accessors for selection parameters
	bool IsParamSelection();
	int NumStates();
	std::vector<std::string>& StateList();
	bool IsSelectionDropdown();

	// Accessors for numeric value parameters
	bool IsParamNumeric();
	double DefaultValue();
	double MinValue();
	double MaxValue();
	double ValueStep();
	std::string& UnitLabel();
	double ValueShapeFactor();

	// Accessors for label data
	// TODO

private:
	//int mCommonParameterType = kUnspecified;

	// Data for all parameter types
	bool mIsParam = false;
	std::string& mParamName = std::string("");
	int mParamIndex;
	int mPosX;
	int mPosY;

	// Data for selection parameters
	bool mIsParamSelection = false;
	std::vector<std::string>& mStateList = std::vector<std::string>();
	bool mIsDropdown;

	// Data for numeric value parameters
	bool mIsParamNumeric = false;
	double mDefaultValue;
	double mMinValue;
	double mMaxValue;
	double mValueStep;
	std::string& mUnitLabel = std::string("");
	double mValueShapeFactor = 1.0;

	// Data for parameters with labels
	bool mIsParamLabeled = false;
	int mLabelBufferV = -10; // space between bottom of bitmap and top of label
	int mLabelBufferH = 0; // one-sided space between edge of label and edge of bitmap (positive value makes label bigger)
	int mLabelHeight = 15;
	int mLabelEditWidth = 60;
	int mLabelFontSize = 18;
	int mLabelEditChars = 5;
};

#endif // ! __PARAMETER_INFO__
