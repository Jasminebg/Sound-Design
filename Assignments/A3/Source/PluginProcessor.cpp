/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "DSPDelayLineTutorial_02.h"
#include "math.h"

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
    //juce::HeapBlock<char> heapblock;
    //tmpblock = juce::dsp::AudioBlock<float>( heapblock, spec.numChannels, spec.maximumBlockSize);
    sRate = sampleRate;
    sBlock = samplesPerBlock;
    outbuffer.setSize(2, sampleRate + samplesPerBlock);
    outbuffer.clear();
    stateVariableFilter.reset();
    fxChain.reset();
    stateVariableFilter.prepare(spec);
    fxChain.prepare(spec);
    constructedIR.prepare(spec);
    gspec = spec;
    updateFX();

}

void A3AudioProcessor::updateFX()
{
    int filterChoice = *apvts.getRawParameterValue("FILTERMENU");
    int phaserChoice = *apvts.getRawParameterValue("PHASERMENU");
    int impulseChoice = *apvts.getRawParameterValue("IMPULSEMENU");
    reverbType = *apvts.getRawParameterValue("REVERBMENU");
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
    float length = *apvts.getRawParameterValue("IRLENGTH");
    //float openfile = *apvts.getRawParameterValue("BUTTON");
    //DBG("openfile " << openfile);
    if (length != irLength) {
        lastIRL = irLength;
        irLength = length;
    }
    else { irLength = length; }


    bypassFilter = false;
    if (filterChoice == 1) stateVariableFilter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    if (filterChoice == 2) stateVariableFilter.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
    if (filterChoice == 3) stateVariableFilter.setType(juce::dsp::StateVariableTPTFilterType::highpass);
    if (filterChoice == 4) bypassFilter = true;
    //stateVariableFilter.setCutoffFrequency(cutoff);

    if (phaserChoice == 1) bypassPhaser = false;
    if (phaserChoice == 2) bypassPhaser = true;

    auto& gainProcessor = fxChain.template get<gainIndex>();
    gainProcessor.setGainLinear(gain);

    auto& phaserProcessor = fxChain.template get<phaserIndex>();
    phaserProcessor.setRate(phaserRate);
    phaserProcessor.setDepth(phaserDepth);
    if (reverbType == 2) {
        auto& verb = fxChain.template get<reverbIndex>();
        auto& revpar = juce::dsp::Reverb::Parameters();
        //revpar.damping = 0;
        revpar.dryLevel = dry;
        //revpar.freezeMode = 0;
        revpar.roomSize = room;
        revpar.wetLevel = wet;
        revpar.width = width;
        verb.setParameters(revpar);
        //DBG("dry " << dry);
        //DBG("room " << room);
        //DBG("wet " << wet);
        //DBG("width " << width);
    }
    //else if(reverbType > 2) {
    //    revpar.dryLevel = 1;
    //    revpar.roomSize = 0.2;
    //    revpar.wetLevel = 1;

    //}

    
    
    if (reverbType == 3) {
  
        //DBG("if " << ((lastir != irfile || !irfile.exists())? "true" : "False" ));
            auto& convolution = fxChain.template get<convoIndex>();
            //juce::File desktop;
            juce::File tempir;
            if (impulseChoice == 1) convolution.reset();
            if (impulseChoice == 2) tempir = desktop.getSpecialLocation(desktop.userDesktopDirectory).getChildFile("Resources").getChildFile("Direct Cabinet N1.wav");
            if (impulseChoice == 3) tempir = desktop.getSpecialLocation(desktop.userDesktopDirectory).getChildFile("Resources").getChildFile("Deep Space.wav");
            if (impulseChoice == 4) tempir = desktop.getSpecialLocation(desktop.userDesktopDirectory).getChildFile("Resources").getChildFile("Chateau de Logne, Outside.wav");
            if (impulseChoice == 5) tempir = desktop.getSpecialLocation(desktop.userDesktopDirectory).getChildFile("Resources").getChildFile("Small Drum Room.wav");
            if (impulseChoice == 6) tempir = desktop.getSpecialLocation(desktop.userDesktopDirectory).getChildFile("Resources").getChildFile("Bottle Hall.wav");
            if (impulseChoice == 7) tempir = desktop.getSpecialLocation(desktop.userDesktopDirectory).getChildFile("Resources").getChildFile("cassette recorder.wav");
            if (impulseChoice == 8) tempir = desktop.getSpecialLocation(desktop.userDesktopDirectory).getChildFile("Resources").getChildFile("guitar amp.wav");
            if (impulseChoice == 9) tempir = userfile;
            //DBG("fc  " << filterChoice);
            //DBG("bp  "<<phaserChoice);
            //DBG("choice " << impulseChoice);

                irfile = tempir;
                if (irfile != lastir  ) {
                    lastir = irfile;
                    //DBG("file " << irfile.getFileName());
                    convolution.reset();
                    convolution.loadImpulseResponse(juce::File(irfile),
                        juce::dsp::Convolution::Stereo::yes,
                        juce::dsp::Convolution::Trim::no,
                        1024);
                }
        //}
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

void A3AudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    int numSamples = buffer.getNumSamples();
    int outNumSamples = outbuffer.getNumSamples();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
        buffer.clear(i, 0, buffer.getNumSamples());
        outbuffer.clear(i, 0, buffer.getNumSamples());
    }

    updateFX();
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::AudioBlock<float> outblock(outbuffer);

    juce::dsp::ProcessContextReplacing<float> context(block);

    juce::dsp::ProcessContextNonReplacing<float> cont(block, outblock);

    if (!bypassFilter) stateVariableFilter.process(context);
    if (!bypassPhaser) fxChain.template get<gainIndex>().process(context);
    //DBG("rt " << reverbType);
    if (reverbType == 2) fxChain.template get<reverbIndex>().process(context);
    if (reverbType == 3) {
        //fxChain.template get<convoIndex>().reset();
        fxChain.template get<convoIndex>().process(cont);
        //block.copyFrom(cont.getInputBlock()).copyFrom(cont.getOutputBlock());
        //block.copyFrom(cont.getOutputBlock()).add(cont.getInputBlock());
        //block.add(block.copyFrom(context.getOutputBlock()));
        for (auto i = 0; i < totalNumInputChannels; ++i) {
            float* data = buffer.getWritePointer(i);
            float* outdata = outbuffer.getWritePointer(i);
            //fillreverb(i, numSamples, outNumSamples, cont.getInputBlock().getChannelPointer(i), outdata);
            fillreverb(i, numSamples, outNumSamples, cont.getOutputBlock().getChannelPointer(i), outdata);
            fillbuffer(buffer, i, numSamples, outNumSamples, outdata);
            //buffer.copyFrom(i, 0, cont.getInputBlock().getChannelPointer(i), numSamples);
            //buffer.copyFrom(i,0, cont.getOutputBlock().getChannelPointer(i), numSamples);
            //buffer.addFrom(i, numSamples, cont.getInputBlock().getChannelPointer(i), numSamples);
        }
    }
    if (reverbType == 4){
        irfile = juce::File();

        //fxChain.template get<convoIndex>().reset();
        if(irLength != lastIRL)  decreasing(buffer);
        //fxChain.template get<convoIndex>().process(cont);
        constructedIR.template get<ccIndex>().process(cont);
        for (auto i = 0; i < totalNumInputChannels; ++i) {
            float* data = buffer.getWritePointer(i);
            float* outdata = outbuffer.getWritePointer(i);
            //DBG("cns" << cont.getOutputBlock().getNumSamples());
            fillreverb(i, numSamples, outNumSamples, cont.getOutputBlock().getChannelPointer(i), outdata);
            fillbuffer(buffer, i, numSamples, outNumSamples, outdata);

            //buffer.addFrom(i, 0, cont.getOutputBlock().getChannelPointer(i), numSamples);
            //buffer.addFrom(i, 0, cont.getInputBlock().getChannelPointer(i), numSamples);
        }
    }
    if (reverbType == 5){
        //auto sd = delay
    }

    WritePosition += numSamples;
    WritePosition %= outNumSamples;

}

void A3AudioProcessor::delayline(juce::AudioBuffer<float>& buffer, juce::dsp::ProcessContextNonReplacing<float> context) {

    float cutoff = *apvts.getRawParameterValue("CUTOFF");
    float delay = *apvts.getRawParameterValue("DELAY");

    //Delay<int, 2> delays;
    //delays.prepare(gspec);
    //delays.setMaxDelayTime(1);
    //delays.setDelayTime(1, delay);
    //delays.setFeedback(1);
    //delays.setWetLevel(1);
    //delays.process(context);
    //DSPTutorialAudioProcessor
    //juce::dsp::DelayLine delays;
    

    
    //stateVariableFilter.setCutoffFrequency(cutoff);

}

void A3AudioProcessor::decreasing(juce::AudioBuffer<float>& buffer) {
    float emir = *apvts.getRawParameterValue("EMULATEDMENU");
    //float irlength = *apvts.getRawParameterValue("IRLENGTH");
    float decayspeed = 0;
    if (emir == 2) decayspeed = -7;
    if (emir == 3) decayspeed = -3.5;
    
    //irBuffer.makeCopyOf(buffer);
    irBuffer.setSize(buffer.getNumChannels(), buffer.getNumSamples()*irLength );
    irBuffer.clear();
    auto& convolution = constructedIR.template get<ccIndex>();
    auto eul = juce::MathConstants<float>::euler;

    for (auto i = 0; i < getTotalNumInputChannels(); ++i) {
        float* data = irBuffer.getWritePointer(i);
        //DBG("num " << irBuffer.getNumSamples());
        for (int i = 0; i < irBuffer.getNumSamples(); ++i) {
            float exponent = (float)(decayspeed * i) / (float)(irBuffer.getNumSamples()* irLength+0.0001);
            float randomvalue = (float)(rand() % 10000) * 0.0001f;
            data[i] = pow(eul, exponent);
            data[i] *= randomvalue;

            //if (data[i] > 0) {
                //DBG("i  " << i);
                //DBG("exp " <<  exponent);
                //DBG("data " << data[i]);
                //DBG("v  " << randomvalue);
            //}
            //data[irBuffer.getNumSamples() - 1 - i] = ( 1 / (float)irBuffer.getNumSamples() )* (float)i;
            //data[irBuffer.getNumSamples() - 1 - i] *= (rand() % 10000) * 0.0001f;
        }
    }
     convolution.loadImpulseResponse(juce::AudioBuffer<float>(irBuffer),sRate,juce::dsp::Convolution::Stereo::yes,
                                    juce::dsp::Convolution::Trim::no,juce::dsp::Convolution::Normalise::no);
}



void A3AudioProcessor::fillreverb(int channel, const int numSamples, const int delayBufferLen,
                                        const float* bufferData, const float* delayBufferData) {

    if (delayBufferLen > numSamples + WritePosition) {
        outbuffer.copyFromWithRamp(channel, WritePosition, bufferData, numSamples, 1, 0.2);
    }
    else {
        const int bufferRemainder = delayBufferLen - WritePosition;
        outbuffer.copyFromWithRamp(channel, bufferRemainder, bufferData, bufferRemainder, 1,0.2);
        outbuffer.copyFromWithRamp(channel, 0, bufferData, delayBufferLen - bufferRemainder,1,0.2);

    }

}

void A3AudioProcessor::fillbuffer(juce::AudioBuffer<float>& buffer, int channel, const int numSamples,
    const int delayBufferLen, const float* delayBufferData) {
    int readPosition = (int) (delayBufferLen + WritePosition - (sRate*1000)) % delayBufferLen;
    //DBG(readPosition);
    if (delayBufferLen > numSamples + readPosition) {
        buffer.addFrom(channel, 0, delayBufferData + readPosition, numSamples);
    }
    else {
        const int delayRemainder = delayBufferLen - WritePosition;
        buffer.addFrom(channel, 0, delayBufferData +readPosition , delayRemainder);
        buffer.addFrom(channel, delayRemainder, delayBufferData, numSamples - delayRemainder);
    }
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
    layout.add(std::make_unique<juce::AudioParameterFloat>("DELAY", "Delay", 0.0f, 1.0f, 0.0f));
    layout.add (std::make_unique<juce::AudioParameterInt> ("FILTERMENU", "Filter Menu", 1, 4, 4));
    layout.add (std::make_unique<juce::AudioParameterInt> ("PHASERMENU", "Phaser Menu", 1, 2, 2));
    layout.add(std::make_unique<juce::AudioParameterInt>("IMPULSEMENU", "Impulse Menu", 1, 9, 1));
    layout.add(std::make_unique<juce::AudioParameterInt>("REVERBMENU", "Reverb Menu", 1, 5, 1));
    layout.add(std::make_unique<juce::AudioParameterInt>("EMULATEDMENU", "Emulated IR Menu", 1, 5, 1));

    //layout.add(std::make_unique<juce::AudioParameterFloat>("DAMPING", "Damp", 0.0f, 1.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("DRYLEVEL", "Dry", 0.0f, 1.0f, 100.0f));
    //layout.add(std::make_unique<juce::AudioParameterFloat>("FREEZE", "Freeze", 0.0f, 1.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("ROOMSIZE", "Room size", 0.0f, 1.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("WET", "Wet", 0.0f, 1.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("WIDTH", "Width", 0.0f, 1.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("IRLENGTH", "Emulated IR Length", 0.0f, 6.0f, 0.0f));
    //layout.add(std::make_unique<juce::AudioParameterBool>("BUTTON", "Button", false));
    //layout.add(std::make_unique<juce::ButtonParameterAttachment>(juce::RangedAudioParameter::, "Button"));
    

    return layout;
}
