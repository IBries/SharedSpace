/*
  ==============================================================================

    SampleSound.h
    Created: 5 May 2018 12:01:13am
    Author:  Isaac Bries

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class SampleSound : public SynthesiserSound
{
public:
	bool appliesToNote(int midiNoteNumber)
	{
		return true;
	}

	//==============================================================================

	bool appliesToChannel(int midiChannel)
	{
		return true;
	}
};