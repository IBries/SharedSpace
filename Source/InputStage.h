/*
  ==============================================================================

    InputStage.h
    Created: 3 May 2018 8:23:31pm
    Author:  Isaac Bries

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================

class InputStage : public Component,
	public ChangeListener,
	public FileDragAndDropTarget
{
public:
	InputStage();
	~InputStage();

	void paint(Graphics&) override;
	void resized() override;

	// Inherited from ChangeListener
	void changeListenerCallback(ChangeBroadcaster *source);

	// Inherited from FileDragAndDropTarget
	bool isInterestedInFileDrag(const StringArray &files);
	void filesDropped(const StringArray &files, int x, int y);

	AudioFormatReader* getAudioFormatReader();

private:
	AudioThumbnail thumbnail;
	AudioFormatManager formatManager;
	AudioFormatReader* reader;
	std::unique_ptr<AudioFormatReaderSource> readerSource;
	AudioThumbnailCache thumbnailCache;
	File audioData;

	void thumbnailChanged();
	void paintIfNoFileLoaded(Graphics& g, const juce::Rectangle<int>& thumbnailBounds);
	void paintIfFileLoaded(Graphics& g, const juce::Rectangle<int>& thumbnailBounds);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InputStage)
};