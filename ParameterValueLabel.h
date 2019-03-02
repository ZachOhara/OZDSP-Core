#ifndef __PARAMETER_VALUE_LABEL__
#define __PARAMETER_VALUE_LABEL__

#include "IPlug_include_in_plug_hdr.h"

#include <string>

class ParameterValueLabel : public ITextControl
{
public:
	ParameterValueLabel(IPlugBase* pPlug, int paramIndex,
		IRECT labelRect, IRECT editRect, int fontSize);
	~ParameterValueLabel();

	void UpdateDisplay();
	void SetDisplayText(char* text);

	void OnMouseDblClick(int x, int y, IMouseMod* pMod) override;
	void TextFromTextEntry(const char* text) override;

private:
	int mParamIndex;
	IParam* mpParam;

	IRECT mEditingRect;
	IText mEditingText;

	static IText ConstructLabelText(int fontSize);
	static IText ConstructEditingText(int fontSize);
	static IText ConstructText(int fontSize, IColor color);
};

#endif // !__PARAMETER_VALUE_LABEL__
