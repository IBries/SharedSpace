/*
  ==============================================================================

    WaveMaker.cpp
    Created: 9 May 2018 12:04:56pm
    Author:  Isaac Bries

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "WaveMaker.h"

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
	AudioFormatReader* reader;
	if (source == &in1 && &in2 != nullptr)
	{
		reader = in1.getAudioFormatReader();
		buffer1.setSize(reader->numChannels, reader->lengthInSamples);
		reader->read(&buffer1, 0, reader->lengthInSamples, 0, true, true);
		convolve();
	}
	else if (source == &in2 && &in1 != nullptr)
	{
		reader = in2.getAudioFormatReader();
		buffer2.setSize(reader->numChannels, reader->lengthInSamples);
		reader->read(&buffer2, 0, reader->lengthInSamples, 0, true, true);
		convolve();
	}
}

void WaveMaker::convolve()
{

}

void WaveMaker::paint (Graphics& g)
{

}

void WaveMaker::resized()
{
	in1.setBounds(0, 0, getWidth(), getHeight() / 2);
	in2.setBounds(0, getHeight() / 2, getWidth(), getHeight() / 2);
}
