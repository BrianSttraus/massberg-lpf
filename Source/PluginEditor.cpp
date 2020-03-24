/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ResonantLpfAudioProcessorEditor::ResonantLpfAudioProcessorEditor (ResonantLpfAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    fcSlider.setRange(100, 5000, 0.1);
    fcSlider.setTextValueSuffix(" Hz");
    fcSlider.setValue(1000);
    fcSlider.addListener(this);
    addAndMakeVisible(fcSlider);
    
    qSlider.setRange(0.5, 20,0.001f);
    qSlider.setValue(0.707);
    qSlider.addListener(this);
    addAndMakeVisible(qSlider);
    
    fcLabel.setText("Fc", dontSendNotification);
    fcLabel.attachToComponent(&fcSlider, true);
    addAndMakeVisible(fcLabel);
    
    qLabel.setText("Q", dontSendNotification);
    qLabel.attachToComponent(&qSlider, true);
    addAndMakeVisible(qLabel);
    
    processor.fcHz = fcSlider.getValue();
    processor.fQ = qSlider.getValue();
    
    
    
    setSize (400, 300);
}

ResonantLpfAudioProcessorEditor::~ResonantLpfAudioProcessorEditor()
{
}

//==============================================================================
void ResonantLpfAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

   
}

void ResonantLpfAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    fcSlider.setBounds(50, 20, getWidth()-50, 30);
    qSlider.setBounds(50, 40, getWidth()-50, 30);
    
    
}

void ResonantLpfAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
    processor.fcHz = fcSlider.getValue();
    processor.fQ = qSlider.getValue();
    
}
