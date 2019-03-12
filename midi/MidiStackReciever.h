#ifndef __MIDI_STACK_RECIEVER_H__
#define __MIDI_STACK_RECIEVER_H__

#include "IPlug_include_in_plug_hdr.h"
#include "IMidiQueue.h"

#include <stack>
#include <queue>

#define NUM_MIDI_NOTES 128

enum NoteState
{
	kNoteInactive, // when a note is neither pressed nor pedaled
	kNotePressed, // when the key is currently down
	kNotePedaled, // when the note is sustained only via pedal
};

enum StatusChangeEventType
{
	kNoteTriggerEventType,
	kNoteReleaseEventType,
	kNoteChangeEventType,
	// TODO pitchbend, modulation, aftertouch, etc.
};

struct NoteStatusEvent
{
	int eventType;
	int noteId;
	int newNoteId;
};

class MidiStackReciever
{
public:
	MidiStackReciever();
	~MidiStackReciever();

	void RecieveMessage(IMidiMsg* pMessage);
	void FlushBlock(int nFrames);
	void AdvanceSample(std::queue<NoteStatusEvent>* eventQueue);

private:
	IMidiQueue mMidiQueue;

	std::stack<int> mNoteIdStack;
	int mNoteStatus[NUM_MIDI_NOTES];

	int mSampleOffset = 0;

	void HandleNoteOn(int noteId);
	void HandleNoteOff(int noteId);
};

#endif // !__MIDI_STACK_RECIEVER_H__
