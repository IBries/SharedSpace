![Logo of the project](https://github.com/IBries/SharedSpace/blob/master/logo.png)
![Logo of the project](https://github.com/IBries/SharedSpace/blob/master/logo.png)
![Logo of the project](https://github.com/IBries/SharedSpace/blob/master/logo.png)
![Logo of the project](https://github.com/IBries/SharedSpace/blob/master/logo.png)
![Logo of the project](https://github.com/IBries/SharedSpace/blob/master/logo.png)

# Shared Space
> a convolutional sampler

Shared Space is (will be) a full-featured sampler, with a twist. The only sound
source available is the convolution of two user-supplied .wav files. Currently, the
JUCE convolution algorithm does not have the performance to do real-time calculations
of longer (>1s signals). Progress is halted until I find a better convolution algorithm.

## Getting started

Drag a .wav file to each of the input containers. Both must have the same number of
channels (i.e. mono/stereo). The convolution is displayed in the output container.

## Current Features
* Input/Output waveform displays
* Accepts MIDI messages (but they don't do anything right now)

## Future Features
* Actual sound!
* Improved MIDI support
* Filtering
* Envolope Generator(s)
* LFO(s)
* Input Transforms (reverse, normalize, etc.)
* Resolution (fft size) control
* Output sectioning, to use only a portion of output as sound source

## Issues
* Convolution is not calculated correctly
* Problematic memory leak solution