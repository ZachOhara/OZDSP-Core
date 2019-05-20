#ifndef __MIDI_STACK_RECIEVER_H__
#define __MIDI_STACK_RECIEVER_H__

#include "IPlug_include_in_plug_hdr.h"
#include "IMidiQueue.h"

#include "TuningProcessor.h"

#include <queue>

enum MidiNoteState
{
	kNoteInactive, // when a note is neither pressed nor pedaled
	kNotePressed, // when the key is currently down
	kNotePedaled, // when the note is sustained only via pedal
};

struct MidiEvent
{
	enum EMidiEventTypes {kNoteBegin, kNoteEnd, kNoteChange};

	int eventType;
	int noteId;
	int newNoteId;
};

class MidiStackReciever
{
public:
	enum EMidiNoteStates {kInactive, kKeyPressed, kPedalSustained};

	MidiStackReciever();
	~MidiStackReciever();

	void RecieveMessage(IMidiMsg* pMessage);
	void FlushBlock(int nFrames);
	void AdvanceSample(std::queue<MidiEvent>* eventQueue);

private:
	IMidiQueue mMidiQueue;

	int mOrderPressed[NUM_MIDI_NOTES];
	int mNoteStatus[NUM_MIDI_NOTES];

	int mSampleOffset = 0;

	void HandleNoteOn(int noteId);
	void HandleNoteOff(int noteId);

	int FindLastSoundingNote();
	void ShiftNoteOrderBack();
	void ShiftNoteOrderForward(int releasedNoteId);
};

#endif // !__MIDI_STACK_RECIEVER_H__
