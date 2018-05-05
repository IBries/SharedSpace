/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "InputStage.h"

//==============================================================================
/**
*/
class SharedSpaceAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    SharedSpaceAudioProcessorEditor (SharedSpaceAudioProcessor&);
    ~SharedSpaceAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

	InputStage* getIn(int inputNumber);

	InputStage in1;
	InputStage in2;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SharedSpaceAudioProcessor& processor;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SharedSpaceAudioProcessorEditor)
};
