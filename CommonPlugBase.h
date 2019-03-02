#ifndef __COMMON_PLUG_BASE__
#define __COMMON_PLUG_BASE__

#include "IPlug_include_in_plug_hdr.h"

#include "ParameterInfo.h"
#include "ParameterValueLabel.h"

#include <map>
#include <string>
#include <vector>

#define COMMONPLUG_CTOR_PARAMS \
	PLUG_CHANNEL_IO, PUBLIC_NAME, PLUG_MFR, PLUG_VER, \
	PLUG_UNIQUE_ID, PLUG_MFR_ID, PLUG_LATENCY, PLUG_DOES_MIDI, \
	PLUG_DOES_STATE_CHUNKS, PLUG_IS_INST, PLUG_SC_CHANS

class CommonPlugBase : public IPlug
{
public:
	CommonPlugBase(
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
		int plugAPI);
	~CommonPlugBase();

	virtual void Reset() override;
	virtual void OnParamChange(int paramIndex) override;
	
	virtual void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames) = 0;

protected:
	virtual void CreatePresets();

	IGraphics* GetGraphics();

	void SetBackground(int id, std::string name);
	void RegisterBitmap(int id, std::string name, int nFrames);

	void ForceUpdateParameters();
	void FinishConstruction();
	void AddParameters(std::vector<ParameterInfo>& paramList);
	void AddParameter(ParameterInfo& param);
	
private:
	IGraphics* mpGraphics;

	std::map<int, IBitmap> mBitmapRegistry;
	std::map<int, ParameterValueLabel*> mLabelRegistry;

	void AddSelectionParameter(ParameterInfo& param, IParam* pParamObj, IBitmap& bitmap);
	void AddNumericParameter(ParameterInfo& param, IParam* pParamObj, IBitmap& bitmap);
	void AddParameterLabel(ParameterInfo& param, IParam* pParamObj, IBitmap& bitmap);

	static IRECT ConstructLabelRect(ParameterInfo& param, IBitmap& bitmap);
	static IRECT ConstructEditingRect(ParameterInfo& param, IRECT& labelRect);
};

#endif // !__COMMON_PLUG_BASE__
