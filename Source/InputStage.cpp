/*
  ==============================================================================

    InputStage.cpp
    Created: 3 May 2018 8:23:32pm
    Author:  Isaac Bries

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "InputStage.h"

//==============================================================================
InputStage::InputStage()
	: thumbnailCache(5),
	  thumbnail (512, formatManager, thumbnailCache)
{
	formatManager.registerBasicFormats();
	thumbnail.addChangeListener(this);
}

InputStage::~InputStage()
{
}

void InputStage::paint (Graphics& g)
{
	juce::Rectangle<int> thumbnailBounds(0, 0, getWidth(), getHeight());

	if (thumbnail.getNumChannels() == 0)
		paintIfNoFileLoaded(g, thumbnailBounds);
	else
		paintIfFileLoaded(g, thumbnailBounds);
}

void InputStage::paintIfNoFileLoaded(Graphics& g, const juce::Rectangle<int>& thumbnailBounds)
{
	g.setColour(Colours::darkgrey);
	g.fillRect(thumbnailBounds);
	g.setColour(Colours::white);
	g.drawFittedText("No File Loaded", thumbnailBounds, Justification::centred, 1.0f);
}

void InputStage::paintIfFileLoaded(Graphics& g, const juce::Rectangle<int>& thumbnailBounds)
{
	g.setColour(Colours::darkblue);
	g.fillRect(thumbnailBounds);
	
	g.setColour(Colours::cornflowerblue);
	thumbnail.drawChannels(g, thumbnailBounds, 0.0, thumbnail.getTotalLength(), 1.0f);
}

void InputStage::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
}

void InputStage::changeListenerCallback(ChangeBroadcaster *source)
{
	if (source == &thumbnail)
		thumbnailChanged();
}

bool InputStage::isInterestedInFileDrag(const StringArray &files)
{
	if (files.size() != 1 || !files.begin()->endsWith(".wav"))
		return false;
	else
		return true;
}

void InputStage::filesDropped(const StringArray &files, int x, int y)
{
	if (isInterestedInFileDrag(files))
	{
		audioData = *(files.begin());
		reader = formatManager.createReaderFor(audioData);

		if (reader != nullptr)
		{
			std::unique_ptr<AudioFormatReaderSource> newSource(new AudioFormatReaderSource(reader, true));
			thumbnail.setSource(new FileInputSource(audioData));
			readerSource.reset(newSource.release());
		}
	}
}

void InputStage::thumbnailChanged()
{
	repaint();
}

AudioFormatReader* InputStage::getAudioFormatReader()
{
	return reader;
}