/*
  ==============================================================================

    WaveMaker.cpp
    Created: 9 May 2018 12:04:56pm
    Author:  Isaac Bries

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "WaveMaker.h"
#include <Windows.h>

//==============================================================================
WaveMaker::WaveMaker()
{
	addAndMakeVisible(&in1);
	addAndMakeVisible(&in2);
	in1.addChangeListener(this);
	in2.addChangeListener(this);
}

WaveMaker::~WaveMaker()
{
}

void WaveMaker::changeListenerCallback(ChangeBroadcaster* source)
{
	if (source == &in1)
	{
		convolve();
	}
	else if (source == &in2)
	{
		convolve();
	}
}

void WaveMaker::convolve()
{
	OutputDebugString("YOU DID IT!");
}

void WaveMaker::paint (Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (Colours::white);
    g.setFont (14.0f);
    g.drawText ("WaveMaker", getLocalBounds(),
                Justification::centred, true);   // draw some placeholder text
}

void WaveMaker::resized()
{
	in1.setBounds(0, 0, getWidth(), getHeight() / 2);
	in2.setBounds(0, getHeight() / 2, getWidth(), getHeight() / 2);
}
