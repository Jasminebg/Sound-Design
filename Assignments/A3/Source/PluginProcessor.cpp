/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "DSPDelayLineTutorial_02.h"

//==============================================================================
A3AudioProcessor::A3AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), apvts(*this, nullptr, "PARAMETERS", createParameterLayout())
#endif
{
}

A3AudioProcessor::~A3AudioProcessor()
{
}

//==============================================================================
const juce::String A3AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool A3AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool A3AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool A3AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double A3AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int A3AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int A3AudioProcessor::getCurrentProgram()
{
    return 0;
}

void A3AudioProcessor::setCurrentProgram (int index)
{
}

const juce::String A3AudioProcessor::getProgramName (int index)
{
    return {};
}

void A3AudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void A3AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getMainBusNumOutputChannels();

    
    stateVariableFilter.reset();
    fxChain.reset();
    stateVariableFilter.prepare(spec);
    fxChain.prepare(spec);
    updateFX();
}

void A3AudioProcessor::updateFX()
{
    int filterChoice = *apvts.getRawParameterValue("FILTERMENU");
    int phaserChoice = *apvts.getRawParameterValue("PHASERMENU");
    int impulseChoice = *apvts.getRawParameterValue("IMPULSEMENU");
    int reverbChoice = *apvts.getRawParameterValue("REVERBMENU");
    float cutoff = *apvts.getRawParameterValue("CUTOFF");
    float phaserRate = *apvts.getRawParameterValue("PHASERRATE");
    float phaserDepth = *apvts.getRawParameterValue("PHASERDEPTH");
    float gain = *apvts.getRawParameterValue("GAIN");

    //float damp = *apvts.getRawParameterValue("DAMPING");
    float dry = *apvts.getRawParameterValue("DRYLEVEL");
    //float freeze = *apvts.getRawParameterValue("FREEZE");
    float room = *apvts.getRawParameterValue("ROOMSIZE");
    float wet = *apvts.getRawParameterValue("WET");
    float width = *apvts.getRawParameterValue("WIDTH");
    //float openfile = *apvts.getRawParameterValue("BUTTON");
    //DBG("openfile " << openfile);


    bypassFilter = false;
    if (filterChoice == 1) stateVariableFilter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    if (filterChoice == 2) stateVariableFilter.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
    if (filterChoice == 3) stateVariableFilter.setType(juce::dsp::StateVariableTPTFilterType::highpass);
    if (filterChoice == 4) bypassFilter = true;
    stateVariableFilter.setCutoffFrequency(cutoff);

    if (phaserChoice == 1) bypassPhaser = false;
    if (phaserChoice == 2) bypassPhaser = true;

    auto& gainProcessor = fxChain.template get<gainIndex>();
    gainProcessor.setGainLinear(gain);

    auto& phaserProcessor = fxChain.template get<phaserIndex>();
    phaserProcessor.setRate(phaserRate);
    phaserProcessor.setDepth(phaserDepth);
    //if (reverbChoice == 1) { 
        auto& verb = fxChain.template get<reverbIndex>();
        auto& revpar = juce::dsp::Reverb::Parameters();
        //revpar.damping = 0;
        revpar.dryLevel = dry;
        //revpar.freezeMode = 0;
        revpar.roomSize = room;
        revpar.wetLevel = wet;
        revpar.width = width;
        verb.setParameters(revpar);
        DBG("dry " << dry);
        DBG("room " << room);
        DBG("wet " << wet);
        DBG("width " << width);
    //}

    
    
    //if (reverbChoice == 2) {
   /*     if (openfile) {
            buttonClicked();
            apvts.getParameter("Button")->setValue(false);
            openfile = false;

        }*/
        juce::File desktop;
        juce::File irfile;

        if (impulseChoice == 2) irfile = desktop.getSpecialLocation(desktop.userDesktopDirectory).getChildFile("Resources").getChildFile("Conic Long Echo Hall.wav");
        if (impulseChoice == 3) irfile = desktop.getSpecialLocation(desktop.userDesktopDirectory).getChildFile("Resources").getChildFile("Highly Damped Large Room.wav");
        if (impulseChoice == 4) irfile = desktop.getSpecialLocation(desktop.userDesktopDirectory).getChildFile("Resources").getChildFile("In a Silo.wav");
        if (impulseChoice == 5) irfile = desktop.getSpecialLocation(desktop.userDesktopDirectory).getChildFile("Resources").getChildFile("Large Widie Echo Hall.wav");
        if (impulseChoice == 6) irfile = desktop.getSpecialLocation(desktop.userDesktopDirectory).getChildFile("Resources").getChildFile("On a Star.wav");
        if (impulseChoice == 7) irfile = userfile;
        //DBG("fc  " << filterChoice);
        //DBG("bp  "<<phaserChoice);
        DBG("choice " << impulseChoice);
        if (irfile.exists()) {
            //convolution.prepare(spec);
            auto& convolution = fxChain.template get<convoIndex>();
            convolution.loadImpulseResponse(irfile,
                juce::dsp::Convolution::Stereo::yes,
                juce::dsp::Convolution::Trim::no,
                1024);
        }
    //}


}


juce::File A3AudioProcessor::buttonClicked()
{
   juce::FileChooser chooser("Select a Wave file to play...", {}, "*.wav");                                        

   if (chooser.browseForFileToOpen())
   {
       auto file = chooser.getResult();
       return file;

   }

}

void A3AudioProcessor::updateParameters()
{
}
void A3AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool A3AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
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

void A3AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    updateFX();
    juce::dsp::AudioBlock<float> block (buffer);
    juce::dsp::ProcessContextReplacing<float> context (block);
    
    if (!bypassFilter) stateVariableFilter.process(context);
    //if (!bypassPhaser) fxChain.process(context);
    fxChain.process(context);
}

//==============================================================================
bool A3AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* A3AudioProcessor::createEditor()
{
    return new A3AudioProcessorEditor (*this);
}

//==============================================================================
void A3AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void A3AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new A3AudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout A3AudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    //int numimp = desktop.getSpecialLocation(desktop.userDesktopDirectory).getChildFile("Resources").getNumberOfChildFiles(3, "*.wav");
 
    layout.add (std::make_unique<juce::AudioParameterFloat> ("CUTOFF", "Cutoff", 20.0f, 20000.0f, 600.0f));
    layout.add (std::make_unique<juce::AudioParameterFloat> ("PHASERRATE", "Rate", 0.0f, 2.0f, 1.0f));
    layout.add (std::make_unique<juce::AudioParameterFloat> ("PHASERDEPTH", "Depth", 0.0f, 1.0f, 0.5f));
    layout.add (std::make_unique<juce::AudioParameterFloat> ("GAIN", "Gain", 0.0f, 2.0f, 1.0f));
    layout.add (std::make_unique<juce::AudioParameterInt> ("FILTERMENU", "Filter Menu", 1, 4, 4));
    layout.add (std::make_unique<juce::AudioParameterInt> ("PHASERMENU", "Phaser Menu", 1, 2, 2));
    layout.add(std::make_unique<juce::AudioParameterInt>("IMPULSEMENU", "Impulse Menu", 1, 7, 1));
    layout.add(std::make_unique<juce::AudioParameterInt>("REVERBMENU", "Reverb Menu", 1, 5, 1));

    //layout.add(std::make_unique<juce::AudioParameterFloat>("DAMPING", "Damp", 0.0f, 1.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("DRYLEVEL", "Dry", 0.0f, 1.0f, 100.0f));
    //layout.add(std::make_unique<juce::AudioParameterFloat>("FREEZE", "Freeze", 0.0f, 1.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("ROOMSIZE", "Room size", 0.0f, 1.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("WET", "Wet", 0.0f, 1.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("WIDTH", "Width", 0.0f, 1.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterBool>("BUTTON", "Button", false));
    //layout.add(std::make_unique<juce::ButtonParameterAttachment>(juce::RangedAudioParameter::, "Button"));
    

    return layout;
}
