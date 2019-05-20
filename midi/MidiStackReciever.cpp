#include "MidiStackReciever.h"

MidiStackReciever::MidiStackReciever()
{
	//mNoteIdStack.emplace(-1); // -1 is for no notes pressed

	for (int i = 0; i < NUM_MIDI_NOTES; i++) {
		mNoteStatus[i] = kNoteInactive;
	}
}

MidiStackReciever::~MidiStackReciever()
{
}

void MidiStackReciever::RecieveMessage(IMidiMsg* pMessage)
{
	mMidiQueue.Add(pMessage);
}

void MidiStackReciever::FlushBlock(int nFrames)
{
	mMidiQueue.Flush(nFrames);
	mSampleOffset = 0;
}

void MidiStackReciever::AdvanceSample(std::queue<MidiEvent>* eventQueue)
{
	// cache the current note
	int oldSoundingNoteId = FindLastSoundingNote();

	// search the messages
	while (!mMidiQueue.Empty() && mMidiQueue.Peek()->mOffset <= mSampleOffset) {
		IMidiMsg* pMessage = mMidiQueue.Peek();
		mMidiQueue.Remove();

		int noteId = pMessage->NoteNumber();

		switch (pMessage->StatusMsg()) {
		case IMidiMsg::kNoteOn:
			// Some midi controllers just set velocity to zero instead
			// of sending a "note off" message
			if (pMessage->Velocity() > 0)
				HandleNoteOn(noteId);
			else
				HandleNoteOff(noteId);
			break;
		case IMidiMsg::kNoteOff:
			HandleNoteOff(noteId);
			break;
		}
	}

	// Find out what changed and queue a corresponding event
	int newSoundingNoteId = FindLastSoundingNote();

	if (oldSoundingNoteId != newSoundingNoteId) {
		MidiEvent event;
		if (oldSoundingNoteId == -1) {
			event.eventType = MidiEvent::kNoteBegin;
			event.noteId = newSoundingNoteId;
		} else if (newSoundingNoteId == -1) {
			event.eventType = MidiEvent::kNoteEnd;
			event.noteId = oldSoundingNoteId;
		} else {
			event.eventType = MidiEvent::kNoteChange;
			event.noteId = oldSoundingNoteId;
			event.newNoteId = newSoundingNoteId;
		}
		eventQueue->emplace(event);
	}

	// Update the offset for the next sample
	mSampleOffset++;
}

void MidiStackReciever::HandleNoteOn(int noteId)
{
	mNoteStatus[noteId] = kNotePressed;
	ShiftNoteOrderBack();
	mOrderPressed[noteId] = 1;
}

void MidiStackReciever::HandleNoteOff(int noteId)
{
	mNoteStatus[noteId] = kNoteInactive;

	ShiftNoteOrderForward(noteId);
}

int MidiStackReciever::FindLastSoundingNote()
{
	for (int i = 0; i < NUM_MIDI_NOTES; i++) {
		if (mOrderPressed[i] == 1) {
			return i;
		}
	}
	return -1;
}

void MidiStackReciever::ShiftNoteOrderBack()
{
	for (int i = 0; i < NUM_MIDI_NOTES; i++) {
		if (mOrderPressed[i] >= 1) {
			mOrderPressed[i] += 1;
		}
	}
}

void MidiStackReciever::ShiftNoteOrderForward(int releasedNoteId)
{
	int releaseIndex = mOrderPressed[releasedNoteId];
	mOrderPressed[releasedNoteId] = 0;
	for (int i = 0; i < NUM_MIDI_NOTES; i++) {
		if (mOrderPressed[i] > releaseIndex) {
			mOrderPressed[i] -= 1;
		}
	}
}
