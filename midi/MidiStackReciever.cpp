#include "MidiStackReciever.h"

MidiStackReciever::MidiStackReciever()
{
	mNoteIdStack.emplace(-1); // -1 is for no notes pressed

	for (int i = 0; i < NUM_MIDI_NOTES; i++)
	{
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

void MidiStackReciever::AdvanceSample(std::queue<NoteStatusEvent>* eventQueue)
{
	// Search through the messages
	int lastSoundingNoteId = mNoteIdStack.top();
	while (!mMidiQueue.Empty() && mMidiQueue.Peek()->mOffset <= mSampleOffset)
	{
		IMidiMsg* pMessage = mMidiQueue.Peek();
		mMidiQueue.Remove();

		int noteId = pMessage->NoteNumber();

		switch (pMessage->StatusMsg())
		{
		case IMidiMsg::kNoteOn:
			HandleNoteOn(noteId);
			break;
		case IMidiMsg::kNoteOff:
			HandleNoteOff(noteId);
			break;
		}
	}
	int newSoundingNoteId = mNoteIdStack.top();

	// Find out what changed and queue it
	if (lastSoundingNoteId != newSoundingNoteId)
	{
		NoteStatusEvent statusEvent;
		if (lastSoundingNoteId == -1)
		{
			statusEvent.eventType = kNoteTriggerEventType;
			statusEvent.noteId = newSoundingNoteId;
		}
		else if (newSoundingNoteId == -1)
		{
			statusEvent.eventType = kNoteReleaseEventType;
			statusEvent.noteId = lastSoundingNoteId;
		}
		else
		{
			statusEvent.eventType = kNoteChangeEventType;
			statusEvent.noteId = lastSoundingNoteId;
			statusEvent.newNoteId = newSoundingNoteId;
		}
		eventQueue->emplace(statusEvent);
	}

	// Update the offset for the next sample
	mSampleOffset++;
}

void MidiStackReciever::HandleNoteOn(int noteId)
{
	mNoteStatus[noteId] = kNotePressed;
	mNoteIdStack.emplace(noteId);
}

void MidiStackReciever::HandleNoteOff(int noteId)
{
	mNoteStatus[noteId] = kNoteInactive;

	while (mNoteIdStack.top() >= 0 && mNoteStatus[mNoteIdStack.top()] == kNoteInactive)
	{
		mNoteIdStack.pop();
	}
}
