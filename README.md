![Logo of the project](https://github.com/IBries/SharedSpace/blob/master/logo.png)

# Shared Space
> a convolutional sampler

Shared Space is (will be) a full-featured sampler, with a twist. The only sound
source available is the convolution of two user-supplied .wav files.

## Getting started

Drag a .wav file to each of the input containers. Both must have the same number of
channels (i.e. mono/stereo). The convolution is displayed in the output container.

## Current Features
* Input/Output waveform displays

## Future Features
* Actual sound!
* MIDI support
* Filtering
* Envolope Generator(s)
* LFO(s)
* Input Transforms (reverse, normalize, etc.)
* Resolution (fft size) control
* Output sectioning, to use only a portion of output as sound source

## Issues
* Convolution is not calculated correctly
* Problematic memory leak solution