/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ResonantLpfAudioProcessor::ResonantLpfAudioProcessor()
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
    
    lowPassFilter.state = new dsp::IIR::Coefficients<float>(b0, b1, b2, a0, a1, a2);
    
    
    
}

ResonantLpfAudioProcessor::~ResonantLpfAudioProcessor()
{
}

//==============================================================================
const String ResonantLpfAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ResonantLpfAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ResonantLpfAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ResonantLpfAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ResonantLpfAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ResonantLpfAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ResonantLpfAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ResonantLpfAudioProcessor::setCurrentProgram (int index)
{
}

const String ResonantLpfAudioProcessor::getProgramName (int index)
{
    return {};
}

void ResonantLpfAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void ResonantLpfAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    lastSampleRate = sampleRate;
    
    dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    
    lowPassFilter.prepare(spec);
    calculateLPFCoeffs(fcHz, fQ);
    lowPassFilter.reset();
    
    
    
    
    
}

void ResonantLpfAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ResonantLpfAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void ResonantLpfAudioProcessor::calculateLPFCoeffs(float fCutoffFreq, float fQ)
{
    float theta_c = 2.0* MathConstants<float>::pi*fCutoffFreq/lastSampleRate;
    float denomFirst = 2.0 - (powf((sqrtf(2.0)*MathConstants<float>::pi)/theta_c, 2.0));
    float denomSecond = powf((2.0* MathConstants<float>::pi)/(fQ*theta_c),2.0);
    float gainOneDenom = sqrtf((powf(denomFirst, 2.0)+denomSecond));
    float gainOne = 2.0/gainOneDenom;
    float omegaS=0;
    if (fQ > sqrtf(0.5)) {
        float gainR = (2.0*powf(fQ, 2.0))/(sqrtf((4.0*powf(fQ, 2.0))-1));
        float wR = sqrtf(1.0-(1.0/(2.0*powf(fQ, 2.0)))) * theta_c;
        float omegaR = tanf(wR/2.0);
        float sNum = powf(gainR, 2.0) - powf(gainOne, 2.0);
        float sDenom = powf(gainR, 2.0) - 1.0;
        omegaS = powf(sNum/sDenom, 1/4) * omegaR;
    }
    else if (fQ <= sqrtf(0.5)){
        float mNumFirst = 2.0 - (1.0/(2.0*powf(fQ, 2.0)));
        float mNumSecond = sqrtf(((1.0-(4.0*powf(fQ, 2.0)))/powf(fQ, 4.0))+(4.0/gainOne));
        float wM = sqrtf((mNumFirst+mNumSecond)/2.0) * theta_c;
        float omegaM = tanf(wM/2.0);
        float omegaeS = (powf(1-powf(gainOne, 2.0), 1/4)*theta_c)/2.0;
        omegaS = (omegaeS<omegaM) ? omegaeS : omegaM;
    }
    
    float poleFreq = 2.0*atanf(omegaS);
    float zeroFreq = 2.0*atanf(omegaS/sqrtf(gainOne));
    
    float gpDenomFirst = 1.0-powf(poleFreq/theta_c, 2.0);
    float gpDenomSecond = powf(poleFreq/(fQ*theta_c), 2.0);
    float gainPole = 1.0/sqrtf(powf(gpDenomFirst, 2.0)+gpDenomSecond);
    
    float gzDenomFirst = 1.0-powf(zeroFreq/theta_c, 2.0);
    float gzDenomSecond = powf(zeroFreq/(fQ*theta_c), 2.0);
    float gainZero = 1.0/sqrtf(powf(gzDenomFirst, 2.0)+gzDenomSecond);
    
    float numQp = (powf(gainPole, 2.0)-powf(gainZero, 2.0))*gainOne;
    float denomQp = (gainOne+powf(gainZero, 2.0))*(powf(gainOne-1.0, 2.0));
    float qPole = sqrtf(numQp/denomQp);
    
    float numQz = (powf(gainPole, 2.0)-powf(gainZero, 2.0))* powf(gainOne, 2.0);
    float denomQz = (gainOne+powf(gainPole, 2.0))*(powf(gainOne-1.0, 2.0))*powf(gainZero, 2.0);
    float qZero = sqrtf(numQz/denomQz);
    
    float gamma = powf(omegaS, 2.0) + (omegaS/qPole) + 1.0;
    float alpha0 = powf(omegaS, 2.0) + ((sqrtf(gainOne)/qZero)*omegaS) + gainOne;
    float alpha1 = 2.0*(powf(omegaS, 2.0)-gainOne);
    float alpha2 = powf(omegaS, 2.0) - ((sqrtf(gainOne)/qZero)*omegaS) + gainOne;
    float beta1 = 2.0*(powf(omegaS, 2.0)-1.0);
    float beta2 = powf(omegaS, 2.0) - (omegaS/qPole) + 1.0;
    
    
    float* coeffs = lowPassFilter.state->coefficients.data();
    
    coeffs[0] = alpha0/gamma;
    coeffs[1] = alpha1/gamma;
    coeffs[2] = alpha2/gamma;
    coeffs[3] = beta1/gamma;
    coeffs[4] = beta2/gamma;
    
}

void ResonantLpfAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    dsp::AudioBlock<float> block (buffer);
    calculateLPFCoeffs(fcHz, fQ);
    lowPassFilter.process(dsp::ProcessContextReplacing<float>(block));

}

//==============================================================================
bool ResonantLpfAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* ResonantLpfAudioProcessor::createEditor()
{
    return new ResonantLpfAudioProcessorEditor (*this);
}

//==============================================================================
void ResonantLpfAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void ResonantLpfAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ResonantLpfAudioProcessor();
}
