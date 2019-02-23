#include "ParamValueLabel.h"

ParamValueLabel::ParamValueLabel(IPlugBase* pPlug, int paramIdx, IRECT pR) :
	ITextControl(pPlug, pR, &defaultParamText, "")
{
	mParamIdx = paramIdx;
	mpParam = pPlug->GetParam(mParamIdx);
	mEntryRect = TextEntryRect();
	UpdateDisplay();
}

ParamValueLabel::~ParamValueLabel()
{
}

void ParamValueLabel::UpdateDisplay()
{
	char* display[MAX_PARAM_DISPLAY_LEN];
	mpParam->GetDisplayForHost((char*) display);
	// add a space
	strcat((char*) display, " ");
	const char* label = mpParam->GetLabelForHost();
	strcat((char*) display, label);
	SetText((char*) display);
}

void ParamValueLabel::SetText(char* text)
{
	SetTextFromPlug(text);
}

void ParamValueLabel::OnMouseDblClick(int x, int y, IMouseMod * pMod)
{
	char* display[MAX_PARAM_DISPLAY_LEN];
	mpParam->GetDisplayForHost((char*) display);

	mPlug->GetGUI()->CreateTextEntry(this, &defaultEditingText, &mEntryRect, (char*) display);
}

void ParamValueLabel::TextFromTextEntry(const char* text)
{
	try
	{
		std::string& sText = std::string(text);
		double value;
		value = std::stod(sText); // this is the line that's going to cause exceptions, if any
		// Set the value to the parameter backend
		mpParam->Set(value);
		// Let the host know the value changed
		mPlug->SetParameterFromGUI(mParamIdx, mpParam->GetNormalized());
		// Let our gui know the value changed
		mPlug->GetGUI()->SetParameterFromPlug(mParamIdx, value, false);
	}
	catch (std::exception& e)
	{
		// Probably just some invalid text entered
		// Ignore it and don't update the value
	}
}

IRECT ParamValueLabel::TextEntryRect()
{
	int dWidth = (mRECT.R - mRECT.L) - pxTextEntryWidth;
	return IRECT(mRECT.L + (dWidth / 2), mRECT.T, mRECT.R - (dWidth / 2), mRECT.B);
}
