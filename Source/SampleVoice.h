/*
  ==============================================================================

    SampleVoice.h
    Created: 5 May 2018 12:01:06am
    Author:  Isaac Bries

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "SampleSound.h"
#include <Windows.h>

class SampleVoice : public SynthesiserVoice
{
public:
	bool canPlaySound(SynthesiserSound* sound)
	{
		return dynamic_cast<SynthesiserSound*>(sound) != nullptr;
	}

	//==============================================================================

	void startNote(int midiNoteNumber, float velocity, SynthesiserSound* sound, int currentPitchWheelPosition)
	{
		frequency = MidiMessage::getMidiNoteInHertz(midiNoteNumber);
		LPCSTR msg = "" + midiNoteNumber;
		OutputDebugString(msg);
	}

	//==============================================================================

	void stopNote(float velocity, bool allowTailOff)
	{
		clearCurrentNote();
	}

	//==============================================================================

	void renderNextBlock(AudioBuffer<float> &outputBuffer, int startSample, int numSamples)
	{

	}

	//==============================================================================

	void pitchWheelMoved(int newPitchWheelValue)
	{

	}

	//==============================================================================

	void controllerMoved(int controllerNumber, int newControllerValue)
	{

	}

	//==============================================================================

private:
	double level;
	double frequency;
};