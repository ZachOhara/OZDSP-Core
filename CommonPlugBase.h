#ifndef __COMMON_PLUG_BASE__
#define __COMMON_PLUG_BASE__

#include "IPlug_include_in_plug_hdr.h"

#include "ParameterInfo.h"

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
	virtual void OnParamChange(int paramIdx) override;
	
	virtual void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames) = 0;

protected:
	virtual void CreatePresets();

	IGraphics* GetGraphics();

	void ForceUpdateParameters();
	void FinishConstruction();
	void InitializeParameter(ParameterInfo& param, IBitmap& bitmap);

private:
	IGraphics* mpGraphics;
};

#endif // !__COMMON_PLUG_BASE__
