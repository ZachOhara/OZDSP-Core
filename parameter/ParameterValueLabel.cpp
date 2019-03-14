#include "ParameterValueLabel.h"

ParameterValueLabel::ParameterValueLabel(IPlugBase* pPlug, int paramIndex, IRECT labelRect, IRECT editRect, int fontSize) :
	ITextControl(pPlug, labelRect, &ConstructLabelText(fontSize))
{
	mParamIndex = paramIndex;
	mpParam = pPlug->GetParam(mParamIndex);
	mEditingRect = editRect;
	mEditingText = ConstructEditingText(fontSize);
}

ParameterValueLabel::~ParameterValueLabel()
{
}

void ParameterValueLabel::UpdateDisplay()
{
	char* display[MAX_PARAM_DISPLAY_LEN];
	// Fetch the display value (numeral only)
	mpParam->GetDisplayForHost((char*) display);
	// Add a space
	strcat((char*) display, " ");
	// Add the unit label
	const char* label = mpParam->GetLabelForHost();
	strcat((char*) display, label);
	// Output result
	SetDisplayText((char*) display);
}

void ParameterValueLabel::SetDisplayText(char* text)
{
	SetTextFromPlug(text);
}

void ParameterValueLabel::OnMouseDblClick(int x, int y, IMouseMod * pMod)
{
	// We don't care about any of these values,
	// only the fact that our label was clicked
	char* currentValue[MAX_PARAM_DISPLAY_LEN];
	mpParam->GetDisplayForHost((char*) currentValue);

	mPlug->GetGUI()->CreateTextEntry(this, &mEditingText, &mEditingRect, (char*) currentValue);
}

void ParameterValueLabel::TextFromTextEntry(const char* text)
{
	try {
		std::string sText(text);
		double value;
		// This is the line that's going to cause exceptions, if any
		value = std::stod(sText);
		// If we get this far, we have a valid new value
		// Set the value to the parameter backend
		mpParam->Set(value);
		// Let the host know the value changed
		mPlug->SetParameterFromGUI(mParamIndex, mpParam->GetNormalized());
		// Let our gui know the value changed
		mPlug->GetGUI()->SetParameterFromPlug(mParamIndex, value, false);
	} catch (std::exception& e) {
		// Probably just some invalid text entered
		// Ignore it and don't update the value
	}
}

IText ParameterValueLabel::ConstructLabelText(int fontSize)
{
	return ConstructText(fontSize, COLOR_WHITE);
}

IText ParameterValueLabel::ConstructEditingText(int fontSize)
{
	return ConstructText(fontSize, COLOR_BLACK);
}

IText ParameterValueLabel::ConstructText(int fontSize, IColor color)
{
	return IText(fontSize, &color, "Roboto", IText::kStyleNormal, IText::kAlignCenter, 0, IText::kQualityDefault);
}
