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
/*
*/
class WaveMaker    : public Component
{
public:
    WaveMaker();
    ~WaveMaker();

    void paint (Graphics&) override;
    void resized() override;

private:
	InputStage in1;
	InputStage in2;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveMaker)
};
