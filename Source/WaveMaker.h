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

	AudioFormatManager formatManager;
	std::unique_ptr<AudioFormatReaderSource> readerSource;
	AudioThumbnailCache thumbnailCache;
	AudioThumbnail outputThumbnail;

	AudioBuffer<float> buffer1;
	AudioBuffer<float> buffer2;
	AudioBuffer<float> output;

	// FFT Implementation
	enum
	{
		fftOrder = 10,
		fftSize = 1 << fftOrder
	};
	dsp::FFT fft;

	int dialDiameter;
	Slider rootNoteSlider;
	Slider positionSlider;

	File outputFile;
	WavAudioFormat* wavFormat;
	int outputSampleRate;
	void outputThumbnailChanged();
	void paintIfNoFileLoaded(Graphics&, const Rectangle<int>&);
	void paintIfFileLoaded(Graphics&, const Rectangle<int>&);

	int calculatePaddedBufferSize();
	AudioBuffer<float> createBucket(AudioBuffer<float>, int, int, int);
	void multiplyBucketsFrequencyDomain(float*, float*, float*);
	void initializeOutputBuffer(bool);
	void convolve();
	void convolve(float*, float*, float*);
	void writeOutputToDisk();
	void displayOutputBuffer();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveMaker)
};
