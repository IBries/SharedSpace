/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "SampleSound.h"
#include "SampleVoice.h"
#include "InputStage.h"

//==============================================================================
SharedSpaceAudioProcessor::SharedSpaceAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
	synth.clearVoices();

	for (int i = 0; i < 5; ++i)
	{
		synth.addVoice(new SampleVoice());
	}

	synth.clearSounds();
	synth.addSound(new SampleSound());
}

SharedSpaceAudioProcessor::~SharedSpaceAudioProcessor()
{
}

//==============================================================================
const String SharedSpaceAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SharedSpaceAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SharedSpaceAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SharedSpaceAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SharedSpaceAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SharedSpaceAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SharedSpaceAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SharedSpaceAudioProcessor::setCurrentProgram (int index)
{
}

const String SharedSpaceAudioProcessor::getProgramName (int index)
{
    return {};
}

void SharedSpaceAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void SharedSpaceAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
	ignoreUnused(samplesPerBlock);
	lastSampleRate = sampleRate;
	synth.setCurrentPlaybackSampleRate(lastSampleRate);
}

void SharedSpaceAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SharedSpaceAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void SharedSpaceAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
	buffer.clear();

	synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumChannels());
}

//==============================================================================
bool SharedSpaceAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* SharedSpaceAudioProcessor::createEditor()
{
    return new SharedSpaceAudioProcessorEditor (*this);
}

//==============================================================================
void SharedSpaceAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void SharedSpaceAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SharedSpaceAudioProcessor();
}
