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
	addAndMakeVisible(&waveMaker);

	setSize (400, 300);
}

//==============================================================================

SharedSpaceAudioProcessorEditor::~SharedSpaceAudioProcessorEditor()
{
}

//==============================================================================

void SharedSpaceAudioProcessorEditor::paint (Graphics& g)
{
}

//==============================================================================

void SharedSpaceAudioProcessorEditor::resized()
{
	waveMaker.setBounds(0, 0, getWidth() / 2, getHeight() / 2);
}