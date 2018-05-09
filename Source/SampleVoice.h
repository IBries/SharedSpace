/*
  ==============================================================================

    SampleVoice.h
    Created: 5 May 2018 12:01:06am
    Author:  Isaac Bries

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
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
		level = velocity;
	}

	//==============================================================================

	void stopNote(float velocity, bool allowTailOff)
	{
		allowTailOff = true;
		if (velocity == 0)
			clearCurrentNote();
	}

	//==============================================================================

	void renderNextBlock(AudioBuffer<float> &outputBuffer, int startSample, int numSamples)
	{
		for (int sample = 0; sample < numSamples; ++sample)
		{
			double theWave = 0.0 * level; // TODO Input Wave

			for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
			{
				outputBuffer.addSample(channel, startSample, theWave);
			}

			++startSample;
		}
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
};