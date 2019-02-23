#ifndef __COMMONPARAMETERS__
#define __COMMONPARAMETERS__

#include "IPlug_include_in_plug_hdr.h"

#include "Oscillator.h"
#include "VolumeControl.h"

// Common types of parameters
void InitVolumeParameter(IParam* pParam);
void MinimizeVolumeParam(IParam* pParam);
void HandleVolumeParamChange(IParam* pParam, VolumeControl* pVolControl);
void InitFrequencyParameter(IParam* pParam);
void InitWaveformParameter(IParam* pParam);

// Parameter tools
void ForceUpdateParams(IPlugBase* pPlug);
bool IsParamMinimized(IParam* pParam);

#endif // !__COMMONPARAMETERS__
