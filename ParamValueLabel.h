#ifndef __PARAMVALUELABEL__
#define __PARAMVALUELABEL__

#include "IPlug_include_in_plug_hdr.h"

#include <string>

static const int pxTextEntryWidth = 60;

static IText defaultParamText(18, &COLOR_WHITE, "Roboto", IText::kStyleNormal, IText::kAlignCenter, 0, IText::kQualityDefault);
static IText defaultEditingText(18, &COLOR_BLACK, "Roboto", IText::kStyleNormal, IText::kAlignCenter, 0, IText::kQualityDefault);

class ParamValueLabel : public ITextControl
{
public:
	ParamValueLabel(IPlugBase* pPlug, int paramIdx, IRECT pR);
	~ParamValueLabel();

	void UpdateDisplay();
	void SetText(char* text);

	void OnMouseDblClick(int x, int y, IMouseMod* pMod) override;
	void TextFromTextEntry(const char* text) override;

private:
	int mParamIdx;
	IParam* mpParam;
	IRECT mEntryRect;

	IRECT TextEntryRect();
};

#endif // !__PARAMVALUELABEL__
