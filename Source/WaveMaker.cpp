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
	: fft(fftOrder),
	  dialDiameter(60),
	  thumbnailCache(5),
	  outputThumbnail(512, formatManager, thumbnailCache)
{
	buffer1.clear();
	buffer2.clear();
	output.clear();

	addAndMakeVisible(&in1);
	addAndMakeVisible(&in2);
	in1.addChangeListener(this);
	in2.addChangeListener(this);

	formatManager.registerBasicFormats();
	outputThumbnail.addChangeListener(this);

	addAndMakeVisible(&rootNoteSlider);
	rootNoteSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
	rootNoteSlider.setTextBoxStyle(Slider::TextBoxBelow, false, rootNoteSlider.getTextBoxWidth(), rootNoteSlider.getTextBoxHeight());
	rootNoteSlider.setRange(0, 127, 1);
	rootNoteSlider.setValue(60);
	addAndMakeVisible(&positionSlider);
	positionSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
	positionSlider.setTextBoxStyle(Slider::TextBoxBelow, false, positionSlider.getTextBoxWidth(), positionSlider.getTextBoxHeight());
	positionSlider.setRange(0, 100, 0.1);
	positionSlider.setValue(50);
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
		initializeOutputBuffer(true);
		convolve();
	}
	else if (source == &in2)
	{
		reader = in2.getAudioFormatReader();
		buffer2 = AudioBuffer<float>(reader->numChannels, reader->lengthInSamples);
		reader->read(&buffer2, 0, reader->lengthInSamples, 0, true, true);
		initializeOutputBuffer(false);
		convolve();
	}
	else if (source == &outputThumbnail)
	{
		outputThumbnailChanged();
	}
}

//==============================================================================

void WaveMaker::outputThumbnailChanged()
{
	repaint();
}

//==============================================================================

void WaveMaker::initializeOutputBuffer(bool isBufferOne)
{
	int numChannels;
	if (isBufferOne)
	{
		numChannels = buffer1.getNumChannels();
	}
	else
	{
		numChannels = buffer2.getNumChannels();
	}
	output = AudioBuffer<float>(numChannels, buffer1.getNumSamples() + buffer2.getNumSamples() - 1);
	output.clear();

}

//==============================================================================

void WaveMaker::convolve()
{
	if (buffer1.getNumChannels() == 2 && buffer2.getNumChannels() == 2)
	{
		// Pads both buffers with zeros in preparation for FFT
		int padSize = buffer1.getNumSamples() + buffer2.getNumSamples() - 1;
		buffer1.setSize(buffer1.getNumChannels(), padSize, true, true, false);
		buffer2.setSize(buffer2.getNumChannels(), padSize, true, true, false);

		for (int channelNumber = 0; channelNumber < 2; ++channelNumber)
		{

			// Upper bound ignores last bucket, which will most likely not be large enough for FFT.
			// The last bucket is most likely zeros anyway, so it shouldn't matter.
			for (int sampleNumber = 0; sampleNumber < buffer1.getNumSamples() - 2 * fft.getSize(); sampleNumber += fft.getSize())
			{
				AudioBuffer<float> bucket1 = AudioBuffer<float>(1, 2 * fft.getSize());
				bucket1.addFrom(0, 0, buffer1, channelNumber, sampleNumber, fft.getSize());
				auto* bucket1ptr = (float*)bucket1.getWritePointer(0);

				AudioBuffer<float> bucket2 = AudioBuffer<float>(1, 2 * fft.getSize());
				bucket2.addFrom(0, 0, buffer2, channelNumber, sampleNumber, fft.getSize());
				auto* bucket2ptr = (float*)bucket2.getWritePointer(0);

				AudioBuffer<float> outputBucket = AudioBuffer<float>(1, 2 * fft.getSize());
				outputBucket.clear();
				auto* outputBucketptr = (float*)outputBucket.getWritePointer(0);

				convolve(outputBucketptr, bucket1ptr, bucket2ptr);

				output.addFrom(channelNumber, sampleNumber, outputBucket, 0, 0, outputBucket.getNumSamples());
			}
		}

		writeOutputToDisk();
		displayOutputBuffer();
	}
}

void WaveMaker::convolve(float* outputBucketptr, float* bucket1ptr, float* bucket2ptr)
{
	fft.performRealOnlyForwardTransform(bucket1ptr);
	fft.performRealOnlyForwardTransform(bucket2ptr);

	for (int i = 0; i < 2*fft.getSize(); ++i)
	{
		outputBucketptr[i] = bucket1ptr[i] * bucket2ptr[i];
	}

	fft.performRealOnlyInverseTransform(outputBucketptr);
}

//==============================================================================

void WaveMaker::writeOutputToDisk()
{
	outputFile = File("C:/Users/Isaac/_Development/C++/SharedSpace/ConvolutionOutput.wav");
	FileOutputStream* outputTo = outputFile.createOutputStream();
	AudioFormatWriter* writer = wavFormat.createWriterFor(outputTo, 44100, output.getNumChannels(), 16, NULL, 0);
	writer->writeFromAudioSampleBuffer(output, 0, output.getNumSamples());
}

//==============================================================================

void WaveMaker::displayOutputBuffer()
{
	auto* reader = formatManager.createReaderFor(outputFile);
	if (reader != nullptr)
	{
		std::unique_ptr<AudioFormatReaderSource> newSource(new AudioFormatReaderSource(reader, true));
		outputThumbnail.setSource(new FileInputSource(outputFile));
		readerSource.reset(newSource.release());
	}
}

//==============================================================================

void WaveMaker::paint (Graphics& g)
{
	juce::Rectangle<int> thumbnailBounds(0, getHeight()/2,  getWidth()-dialDiameter, getHeight()/2);

	if (outputThumbnail.getNumChannels() == 0)
		paintIfNoFileLoaded(g, thumbnailBounds);
	else
		paintIfFileLoaded(g, thumbnailBounds);
}

//==============================================================================

void WaveMaker::paintIfNoFileLoaded(Graphics& g, const Rectangle<int>& thumbnailBounds)
{
	g.setColour(Colours::darkgrey);
	g.fillRect(thumbnailBounds);
	g.setColour(Colours::white);
	g.drawFittedText("No File Loaded", thumbnailBounds, Justification::centred, 1.0f);
}

//==============================================================================

void WaveMaker::paintIfFileLoaded(Graphics& g, const Rectangle<int>& thumbnailBounds)
{
	g.setColour(Colours::darkblue);
	g.fillRect(thumbnailBounds);

	g.setColour(Colours::cornflowerblue);
	outputThumbnail.drawChannels(g, thumbnailBounds, 0.0, outputThumbnail.getTotalLength(), 1.0f);
}

//==============================================================================

void WaveMaker::resized()
{
	in1.setBounds(0, 0, (getWidth() - dialDiameter)/2, getHeight() / 2);
	in2.setBounds((getWidth() - dialDiameter) / 2, 0, (getWidth() - dialDiameter)/2, getHeight() / 2);
	rootNoteSlider.setBounds(getWidth() - dialDiameter, 0, dialDiameter, getHeight() / 2);
	positionSlider.setBounds(getWidth() - dialDiameter, getHeight() / 2, dialDiameter, getHeight() / 2);
}
