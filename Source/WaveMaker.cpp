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
	: dialDiameter(60),
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
	outputFile.deleteFile();
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
		createAndDisplayOutput();
	}
	else if (source == &in2)
	{
		reader = in2.getAudioFormatReader();
		buffer2 = AudioBuffer<float>(reader->numChannels, reader->lengthInSamples);
		reader->read(&buffer2, 0, reader->lengthInSamples, 0, true, true);
		createAndDisplayOutput();
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

void WaveMaker::createAndDisplayOutput()
{
	if (buffer1.getNumChannels() ==  buffer2.getNumChannels())
	{
		int fftSize = calculateFFTSize(buffer1.getNumSamples(), buffer2.getNumSamples());
		int fftOrder = log2(fftSize);

		padBuffersWithZeros(2 * fftSize);
		initializeOutputBuffer(2 * fftSize);
		
		for (int channel = 0; channel < buffer1.getNumChannels(); ++channel)
		{
			splitBuffersIntoChannels(channel, 2 * fftSize);
			auto* buffer1SingleChannelPtr = buffer1SingleChannel.getWritePointer(0);
			auto* buffer2SingleChannelPtr = buffer2SingleChannel.getWritePointer(0);
			auto* outputSingleChannelPtr = outputSingleChannel.getWritePointer(0);

			convolve(buffer1SingleChannelPtr, buffer2SingleChannelPtr, outputSingleChannelPtr, fftOrder);

			output.copyFrom(channel, 0, outputSingleChannelPtr, outputSingleChannel.getNumSamples() / 2);
		}

		processOutput();
		writeOutputToDisk();
		displayOutputBuffer();
	}
}

//==============================================================================

int WaveMaker::calculateFFTSize(int length1, int length2)
{
	int fftSize = 1;
	
	// Finds the smallest power of 2 larger than the size of the convolution
	while (fftSize <= length1 + length2 - 1)
	{
		fftSize = fftSize << 1;
	}

	return fftSize;	
}

//==============================================================================

void WaveMaker::padBuffersWithZeros(int newSize)
{
	buffer1.setSize(buffer1.getNumChannels(), newSize, true, true);
	buffer2.setSize(buffer2.getNumChannels(), newSize, true, true);
}

//==============================================================================

void WaveMaker::initializeOutputBuffer(int size)
{
	output = AudioBuffer<float>(buffer1.getNumChannels(), size / 2);
	outputSingleChannel = AudioBuffer<float>(1, size);
}

//==============================================================================

void WaveMaker::splitBuffersIntoChannels(int channel, int size)
{
	buffer1SingleChannel = AudioBuffer<float>(1, size);
	buffer2SingleChannel = AudioBuffer<float>(1, size);

	buffer1SingleChannel.copyFrom(0, 0, buffer1, channel, 0, size);
	buffer2SingleChannel.copyFrom(0, 0, buffer2, channel, 0, size);
}

//==============================================================================

void WaveMaker::convolve(float* in1, float* in2, float* out, int order)
{
	dsp::FFT fft(order);

	fft.performRealOnlyForwardTransform(in1);
	fft.performRealOnlyForwardTransform(in2);

	complexMultiply(in1, in2, out, fft.getSize());

	fft.performRealOnlyInverseTransform(out);
}

//==============================================================================

void WaveMaker::complexMultiply(float* in1, float* in2, float* out, int length)
{
	for (int i = 0; i < 2 * length - 1; ++i)
	{
		std::complex<float> complex1(in1[i], in2[i + 1]);
		std::complex<float> complex2(in2[i], in2[i + 1]);
		std::complex<float> complexOut = complex1 * complex2;

		out[i] = complexOut.real();
		out[i + 1] = complexOut.imag();
	}
}

//==============================================================================

void WaveMaker::processOutput()
{
	float fadeLength = 0.01;
	int fadeLengthInSamples = 44100 * fadeLength;
	output.applyGainRamp(0, fadeLengthInSamples, 0, 1);
	output.applyGainRamp(output.getNumSamples() - fadeLengthInSamples, fadeLengthInSamples, 1, 0);
	output.applyGain(0.8 / output.getMagnitude(0, output.getNumSamples()));
}

//==============================================================================

void WaveMaker::writeOutputToDisk()
{
	outputFile.deleteFile(); // Currently isn't working

	wavFormat = new WavAudioFormat();

	String filePath = File::getCurrentWorkingDirectory().getFullPathName();
	outputFile = File(filePath + "\\ConvolutionOutput.wav");
	FileOutputStream* outputTo = outputFile.createOutputStream();
	AudioFormatWriter* writer = wavFormat->createWriterFor(outputTo, 44100, output.getNumChannels(), 16, NULL, 0);
	writer->writeFromAudioSampleBuffer(output, 0, output.getNumSamples());
	delete writer; // Not ideal, but I don't feel like implementing scoped pointers right now
	delete wavFormat; // Not ideal, but I don't feel like implementing scoped pointers right now
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
