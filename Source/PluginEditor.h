/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class ResonantLpfAudioProcessorEditor  : public AudioProcessorEditor,
                                         public Slider::Listener
{
public:
    ResonantLpfAudioProcessorEditor (ResonantLpfAudioProcessor&);
    ~ResonantLpfAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    void sliderValueChanged(Slider* slider) override;
private:
    
    Slider fcSlider, qSlider;
    Label fcLabel, qLabel;
    ResonantLpfAudioProcessor& processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ResonantLpfAudioProcessorEditor)
};
