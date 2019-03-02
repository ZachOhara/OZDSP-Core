#ifndef __COMMON_PLUG_BASE_H__
#define __COMMON_PLUG_BASE_H__

#include "IPlug_include_in_plug_hdr.h"
#include "resource.h"

#include "parameter/ParameterDataBridge.h"
#include "parameter/ParameterInfo.h"
#include "parameter/ParameterValueLabel.h"

#include "processing/Oscillator.h"
#include "processing/ToneProcessor.h"
#include "processing/VolumeProcessor.h"

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

	void AddOscillatorFrequencyBridge(int paramIndex, Oscillator* pProcessor);
	void AddOscillatorWaveformBridge(int paramIndex, Oscillator* pProcessor);
	void AddVolumeParamBridge(int paramIndex, VolumeProcessor* pProcessor);

	void AddParameters(std::vector<ParameterInfo>& paramList);
	void AddParameter(ParameterInfo& param);

	void ForceUpdateParameters();
	void FinishConstruction();
	
private:
	IGraphics* mpGraphics;

	std::map<int, IBitmap> mBitmapRegistry;
	std::map<int, ParameterValueLabel*> mLabelRegistry;

	std::vector<std::pair<int, ParameterDataBridge*>> mDataBridgeList;

	void AddParameterBridge(int paramIndex, ParameterDataBridge* pBridge);

	void AddSelectionParameter(ParameterInfo& param, IParam* pParamObj, IBitmap& bitmap);
	void AddNumericParameter(ParameterInfo& param, IParam* pParamObj, IBitmap& bitmap);
	void AddParameterLabel(ParameterInfo& param, IParam* pParamObj, IBitmap& bitmap);

	static IRECT ConstructLabelRect(ParameterInfo& param, IBitmap& bitmap);
	static IRECT ConstructEditingRect(ParameterInfo& param, IRECT& labelRect);
};

#endif // !__COMMON_PLUG_BASE_H__
