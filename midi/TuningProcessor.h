#ifndef __TUNING_PROCESSOR_H__
#define __TUNING_PROCESSOR_H__

#include "../ModularProcessor.h"

#define NUM_MIDI_NOTES 128
#define NUM_KEYS 12

class TuningProcessor : public ModularProcessor
{
public:
	enum EParameters {kTemperamentParam, kKeyParam, kNumParams};
	enum ETemperaments {kEqualTemperament, kJustTemperamentMajor, kJustTemperamentMinor};

	TuningProcessor();
	~TuningProcessor();

	double GetFrequencyOfNote(int noteId);

	void SetTemperament(int temperament);
	void SetKey(int key);

private:
	const int kMiddleA_id = 57;
	const double kMiddleA_hz = 440.0;
	const int justifiedMajorRatios[12][2] = {
		{1, 1}, {16, 15}, {9, 8}, {6, 5}, {5, 4}, {4, 3},
		{45, 32}, {3, 2}, {8, 5}, {5, 3}, {9, 5}, {15, 8}
	};

	double equalTemperamentTunings[NUM_MIDI_NOTES];
	double justTemperamentTunings[NUM_KEYS][NUM_MIDI_NOTES];

	int mTuningMode;
	int mCurrentKey;
	int mRelativeMajorKey;

	void InitEqualTemperamentTunings();
	void InitJustTemperamentTunings();
};

#endif // !__TUNING_PROCESSOR_H__
