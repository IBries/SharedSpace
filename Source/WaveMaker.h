/*
  ==============================================================================

    WaveMaker.h
    Created: 9 May 2018 12:04:56pm
    Author:  Isaac Bries

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "InputStage.h"

//==============================================================================

class WaveMaker    : public Component,
					 public ChangeListener
{
public:
    WaveMaker();
    ~WaveMaker();

    void paint (Graphics&) override;
    void resized() override;

	void changeListenerCallback(ChangeBroadcaster*);

private:
	InputStage in1;
	InputStage in2;

	AudioBuffer<float> buffer1;
	AudioBuffer<float> buffer2;
	AudioBuffer<float> output;

	void initializeOutputBuffer();
	void convolve();
	void convolve(float*, float*, float*);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveMaker)
};
