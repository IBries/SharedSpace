/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SharedSpaceAudioProcessorEditor::SharedSpaceAudioProcessorEditor (SharedSpaceAudioProcessor& p)
    : AudioProcessorEditor (&p),
	  processor (p)
{
	addAndMakeVisible(&in1);
	addAndMakeVisible(&in2);
	
	setSize (400, 300);
}

SharedSpaceAudioProcessorEditor::~SharedSpaceAudioProcessorEditor()
{
}

//==============================================================================
void SharedSpaceAudioProcessorEditor::paint (Graphics& g)
{
}

void SharedSpaceAudioProcessorEditor::resized()
{
	in1.setBounds(0, 0, getWidth(), getHeight()/2);
	in2.setBounds(0, getHeight()/2, getWidth(), getHeight()/2);
}


InputStage* SharedSpaceAudioProcessorEditor::getIn(int inputNumber)
{
	if (inputNumber == 1)
		return &in1;
	else if (inputNumber == 2)
		return &in2;
}