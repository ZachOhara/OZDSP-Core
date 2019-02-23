#include "CommonParameters.h"

bool IsParamMinimized(IParam* pParam)
{
	return (pParam->Value() == pParam->GetMin());
}

void InitVolumeParameter(IParam* pParam)
{
	const double min_db = -100.0;
	const double max_db = 0.0;
	const double default_db = 0.0;

	pParam->InitDouble("Volume", default_db, min_db, max_db, 0.1, "dB");
	pParam->SetShape(0.2); // determined experimentally
	pParam->SetDisplayText(-100, "-oo dB");
}

void ZeroVolumeParam(IParam* pParam)
{
	const double threshold_db = -99.95;
	if (pParam->Value() <= threshold_db)
	{
		// First check if it's already at min, otherwise we'll get infinite recursion
		if (pParam->Value() != pParam->GetMin())
		{
			pParam->Set(pParam->GetMin());
		}
	}
}
