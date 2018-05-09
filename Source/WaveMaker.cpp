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

WaveMaker::WaveMaker() :
	buffer1(2, 512),
	buffer2(2, 512)
{
	buffer1.clear();
	buffer2.clear();

	addAndMakeVisible(&in1);
	addAndMakeVisible(&in2);
	in1.addChangeListener(this);
	in2.addChangeListener(this);
}

WaveMaker::~WaveMaker()
{
}

//==============================================================================

void WaveMaker::changeListenerCallback(ChangeBroadcaster* source)
{
	AudioFormatReader* reader;
	if (source == &in1)
	{
		reader = in1.getAudioFormatReader();
		buffer1 = AudioBuffer<float>(reader->numChannels, reader->lengthInSamples);
		reader->read(&buffer1, 0, reader->lengthInSamples, 0, true, true);
	}
	else if (source == &in2)
	{
		reader = in2.getAudioFormatReader();
		buffer2 = AudioBuffer<float>(reader->numChannels, reader->lengthInSamples);
		reader->read(&buffer2, 0, reader->lengthInSamples, 0, true, true);
	}

	initializeOutputBuffer();
	convolve();
}

//==============================================================================

void WaveMaker::initializeOutputBuffer()
{
	output = AudioBuffer<float>(2, buffer1.getNumSamples() + buffer2.getNumSamples() - 1);
}

//==============================================================================

void WaveMaker::convolve()
{
	if (&buffer1 != nullptr && &buffer2 != nullptr)
	{
		for (int channelNumber = 0; channelNumber < 2; ++channelNumber)
		{
			auto* outptr = (float*) output.getWritePointer(channelNumber);
			auto* in1ptr = (float*) buffer1.getReadPointer(channelNumber);
			auto* in2ptr = (float*) buffer2.getReadPointer(channelNumber);
			convolve(outptr, in1ptr, in2ptr);
		}
	}
}

void WaveMaker::convolve(float* outptr, float* in1ptr, float* in2ptr)
{
	//TODO Re-implement using FFT
	for (int i = 0; i < buffer1.getNumSamples(); ++i)
	{
		outptr[i] = 0;
		for (int j = 0; j < buffer2.getNumSamples(); ++j)
		{
			outptr[i] += in1ptr[i - j] * in2ptr[j];
		}
	}
}

//==============================================================================

void WaveMaker::paint (Graphics& g)
{

}

//==============================================================================

void WaveMaker::resized()
{
	in1.setBounds(0, 0, getWidth(), getHeight() / 2);
	in2.setBounds(0, getHeight() / 2, getWidth(), getHeight() / 2);
}
