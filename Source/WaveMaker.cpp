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
	if (buffer1.getNumChannels() ==  buffer2.getNumChannels())
	{
		// Pad buffers so we can split them both into n buckets, where n is an integer
		int newTotalNumSamples = calculatePaddedBufferSize();
		buffer1.setSize(buffer1.getNumChannels(), newTotalNumSamples, true, true, false);
		buffer2.setSize(buffer2.getNumChannels(), newTotalNumSamples, true, true, false);

		for (int channel = 0; channel < buffer1.getNumChannels(); ++channel)
		{
			for (int sample = 0; sample < buffer1.getNumSamples(); sample += fft.getSize())
			{
				AudioBuffer<float> buffer1Bucket = createBucket(buffer1, channel, sample, fft.getSize());
				AudioBuffer<float> buffer2Bucket = createBucket(buffer2, channel, sample, fft.getSize());
				AudioBuffer<float> outputBucket = AudioBuffer<float>(buffer1Bucket.getNumChannels(), 2 *fft.getSize());
				outputBucket.clear();

				// Pad Buckets with zeros
				buffer1Bucket.setSize(buffer1Bucket.getNumChannels(), 2 * fft.getSize(), true, true, false);
				buffer2Bucket.setSize(buffer2Bucket.getNumChannels(), 2 * fft.getSize(), true, true, false);

				auto* buffer1BucketPtr = buffer1Bucket.getWritePointer(0);
				auto* buffer2BucketPtr = buffer2Bucket.getWritePointer(0);
				auto* outputBucketPtr = outputBucket.getWritePointer(0);

				fft.performRealOnlyForwardTransform(buffer1BucketPtr);
				fft.performRealOnlyForwardTransform(buffer2BucketPtr);

				multiplyBucketsFrequencyDomain(buffer1BucketPtr, buffer2BucketPtr, outputBucketPtr);

				fft.performRealOnlyInverseTransform(outputBucketPtr);

				output.copyFrom(channel, sample, outputBucket, 0, 0, fft.getSize());
			}
		}

		// Normalize Output
		output.applyGain(0.8 / output.getMagnitude(0, output.getNumSamples()));

		writeOutputToDisk();
		displayOutputBuffer();
	}
}

//==============================================================================

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

int WaveMaker::calculatePaddedBufferSize()
{
	int sizeOfLargerBuffer;
	if (buffer1.getNumSamples() >= buffer2.getNumSamples())
	{
		sizeOfLargerBuffer = buffer1.getNumSamples();
	}
	else
	{
		sizeOfLargerBuffer = buffer2.getNumSamples();
	}


	while (sizeOfLargerBuffer % fft.getSize() != 0)
	{
		sizeOfLargerBuffer++;
	}
	return sizeOfLargerBuffer;
}

//==============================================================================

AudioBuffer<float> WaveMaker::createBucket(AudioBuffer<float> source, int channel, int startSample, int bucketSize)
{
	jassert(channel < source.getNumChannels() && channel >= 0);
	
	AudioBuffer<float> bucket = AudioBuffer<float>(1, fft.getSize());
	bucket.clear();
	
	bucket.copyFrom(0, 0, source, channel, startSample, bucketSize);

	return bucket;
}

//==============================================================================

void WaveMaker::multiplyBucketsFrequencyDomain(float* in1, float* in2, float* output)
{
	for (int i = 0; i < 2 * fft.getSize(); i += 2)
	{
		std::complex<float> val1 = std::complex<float>(in1[i], in1[i+1]);
		std::complex<float> val2 = std::complex<float>(in2[i], in2[i+1]);
		std::complex<float> out = val1 * val2;

		output[i] = std::real(out);
		output[i+1] = std::imag(out);
	}
}

//==============================================================================

void WaveMaker::writeOutputToDisk()
{
	wavFormat = new WavAudioFormat();

	String filePath = File::getCurrentWorkingDirectory().getFullPathName();
	outputFile = File(filePath + "\\ConvolutionOutput.wav");
	FileOutputStream* outputTo = outputFile.createOutputStream();
	AudioFormatWriter* writer = wavFormat->createWriterFor(outputTo, 44100, output.getNumChannels(), 16, NULL, 0);
	writer->writeFromAudioSampleBuffer(output, 0, output.getNumSamples());
	delete writer;
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
