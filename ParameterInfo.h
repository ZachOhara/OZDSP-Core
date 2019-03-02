#ifndef  __PARAMETER_INFO__
#define __PARAMETER_INFO__

#include "IPlug_include_in_plug_hdr.h"

#include <string>
#include <vector>

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
	ParameterInfo& InitParam(std::string name, int paramIdx, int bitmapId, int posX, int posY);
	ParameterInfo& InitSelectionParam(std::vector<std::string> states, bool isDropdown);
	ParameterInfo& InitNumericParam(double defaultValue, double min, double max,
		double step, std::string unit);
	ParameterInfo& InitLabel();

	// Setters for specific settings
	ParameterInfo& SetValueShapeFactor(double weightFactor);
	ParameterInfo& AddSpecialDisplayValue(int value, std::string display);
	ParameterInfo& SetLabelEditWidth(int editWidth);
	ParameterInfo& SetLabelEditChars(int editChars);

	// Accessors for all parameter types
	bool IsParam();
	std::string& Name();
	int ParamIndex();
	int BitmapId();
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
	std::vector<std::pair<int, std::string>>& SpecialDisplayValues();

	// Accessors for label data
	bool IsParamLabeled();
	int LabelVerticalBuffer();
	int LabelWidthBuffer();
	int LabelHeight();
	int LabelEditWidth();
	int LabelEditChars();
	int LabelFontSize();

private:
	// Note: the project will need to be cleaned / rebuilt if these defaults change

	// Data for all parameter types
	bool mIsParam = false;
	std::string mParamName;
	int mParamIndex;
	int mBitmapId;
	int mPosX;
	int mPosY;

	// Data for selection parameters
	bool mIsParamSelection = false;
	std::vector<std::string> mStateList;
	bool mIsDropdown;

	// Data for numeric value parameters
	bool mIsParamNumeric = false;
	double mDefaultValue;
	double mMinValue;
	double mMaxValue;
	double mValueStep;
	std::string mUnitLabel;
	double mValueShapeFactor = 1.0;
	std::vector<std::pair<int, std::string>> mSpecialDisplayValues;

	// Data for parameters with labels
	bool mIsParamLabeled = false;
	int mLabelVerticalBuffer = -10; // space between bottom of bitmap and top of label
	int mLabelWidthBuffer = 0; // two-sided space between edge of label and edge of bitmap (positive value makes label bigger)
	int mLabelHeight = 15; // based on font size
	int mLabelEditWidth = 60; // should be big enough to fully cover the label text
	int mLabelEditChars = 6; // based on edit width
	int mLabelFontSize = 18;
};

#endif // ! __PARAMETER_INFO__
