#include "TuningProcessor.h"

TuningProcessor::TuningProcessor() :
	ModularProcessor(kNumParams)
{
	InitEqualTemperamentTunings();
	InitJustTemperamentTunings();

	SetTemperament(kEqualTemperament);
	SetKey(3); // default to C
}

TuningProcessor::~TuningProcessor()
{
}

double TuningProcessor::GetFrequencyOfNote(int noteId)
{
	switch (mTuningMode) {
	case kEqualTemperament:
		return equalTemperamentTunings[noteId];
	case kJustTemperamentMajor:
		return justTemperamentTunings[mCurrentKey][noteId];
	case kJustTemperamentMinor:
		return justTemperamentTunings[mRelativeMajorKey][noteId];
	default:
		return 0;
	}
	
}

void TuningProcessor::SetTemperament(int tuningMode)
{
	mTuningMode = tuningMode;
}

void TuningProcessor::SetKey(int key)
{
	mCurrentKey = key;
	// Set the relative major (only applicable to minor keys)
	mRelativeMajorKey = key + 3;
	if (mRelativeMajorKey >= NUM_KEYS)
		mRelativeMajorKey -= NUM_KEYS;
}

void TuningProcessor::InitEqualTemperamentTunings()
{
	for (int i = 0; i < NUM_MIDI_NOTES; i++) {
		double dSemitones = i - kMiddleA_id;
		equalTemperamentTunings[i] = kMiddleA_hz * std::pow(2, dSemitones / 12.0);
	}
}

void TuningProcessor::InitJustTemperamentTunings()
{
	for (int key = 0; key < NUM_KEYS; key++) {
		const int center_id = kMiddleA_id + key;

		// Set the center key tuning from equal temperament
		justTemperamentTunings[key][center_id] = equalTemperamentTunings[center_id];

		// Set the octave above the center key
		for (int i = 1; i < 12; i++) {
			justTemperamentTunings[key][center_id + i] = justTemperamentTunings[key][center_id]
				* justifiedMajorRatios[i][0] / justifiedMajorRatios[i][1];
		}

		// Set the rest of the notes based on the center octave
		for (int i = 0; i < NUM_MIDI_NOTES; i++) {
			int semitoneOffset = i - center_id;
			int octaveOffset = (int) std::floor(((double)semitoneOffset) / 12.0);
			justTemperamentTunings[key][i] = justTemperamentTunings[key][i - (octaveOffset * 12)] * std::pow(2, octaveOffset);
		}
	}
}
