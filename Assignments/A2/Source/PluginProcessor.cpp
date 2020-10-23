/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/
//Used this video tutorial series to help me figure out how to use the various functions in here, after figuring out how it's used I had to
//figure out for myself how to take it and use it for this assignment specifically rather than how it was used in the tutorial
//https://www.youtube.com/watch?v=IRFUYGkMV8w


#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
A2StarterAudioProcessor::A2StarterAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    )
#endif
{
}

A2StarterAudioProcessor::~A2StarterAudioProcessor()
{
}

//==============================================================================
const juce::String A2StarterAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool A2StarterAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool A2StarterAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool A2StarterAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double A2StarterAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int A2StarterAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int A2StarterAudioProcessor::getCurrentProgram()
{
    return 0;
}

void A2StarterAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String A2StarterAudioProcessor::getProgramName(int index)
{
    return {};
}

void A2StarterAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void A2StarterAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    rate = static_cast<float> (sampleRate);
    //volumeBoost = 1.0;
    dry = 100;
    lastFeed = 1;
    wet, feedback, lastping, time = 0;
    pingpong = false;

    // Need to change this value to a number that corresponds to 3 seconds
    delayBufferLength = (int)(3 * (sampleRate + samplesPerBlock));

    delayBuffer.setSize(2, delayBufferLength);
    delayBuffer.clear();
    //float delayInSamples = time * sampleRate;

    //delayWritePosition = 0;
    //delayReadPosition = (int)(delayWritePosition -
      //  delayInSamples + delayBufferLength) % delayBufferLength;


}

void A2StarterAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool A2StarterAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
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

void A2StarterAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    int numSamples = buffer.getNumSamples();
    int delayNumSamples = delayBuffer.getNumSamples();
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
        buffer.clear(i, 0, numSamples);
        delayBuffer.clear(i, 0, delayNumSamples);
    }


    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
     //   if (pingpong && lastping == channel) {}
      //  else {

            double channelfeedback = feedback / 100;



            float* data = buffer.getWritePointer(channel);


            const float* bufferData = buffer.getReadPointer(channel);
            const float* delayBufferData = delayBuffer.getReadPointer(channel);


            for (int i = 0; i < numSamples; ++i) {
                data[i] = data[i] * dry / 100;

            }

            //Wet delay, only called if wet > 0 to not make a greater load
            if (wet > 0) {
                 //currently has an issue where if you increase the time interval fast enough 
                 //right after the wet sound, it will play again at that new interval

                fillDelayBuff(channel, numSamples, delayNumSamples, bufferData, delayBufferData, wet/100, wet / 100);
                getDelayBuff(buffer, channel, numSamples, delayNumSamples, bufferData, delayBufferData);
                /*for (int i = 0; i < numSamples; ++i) {
                    DBG("data" << data[i]);

                }*/
            }
            if (channelfeedback > 0) {
                //DBG("feedback" << feedback);
                FeedbackDelay(channel, numSamples, delayNumSamples, bufferData, data, channelfeedback, channelfeedback);
                getDelayBuff(buffer, channel, numSamples, delayNumSamples, bufferData, delayBufferData);

              /*  for (int i = 0; i < numSamples; ++i) {
                    DBG("data2" << data[i]);

                }*/
            }
         //   lastping = channel;
      //  }
          //DBG("feedback" << feedback);

    }
    WritePosition += numSamples;
    WritePosition %= delayNumSamples;


}

void A2StarterAudioProcessor::fillDelayBuff(int channel, const int numSamples, const int delayNumSamples,
    const float* bufferData, const float* delayBufferData, const double gainstart, const double gainend) {
    if (delayNumSamples > numSamples + WritePosition) {
        delayBuffer.copyFromWithRamp(channel, WritePosition, bufferData, numSamples, gainstart, gainend);
    }
    else {
        const int bufferRemainder = delayNumSamples - WritePosition;
        delayBuffer.copyFromWithRamp(channel, WritePosition, bufferData, bufferRemainder, gainstart, gainend);
        delayBuffer.copyFromWithRamp(channel, 0, bufferData, numSamples - bufferRemainder, gainstart, gainend);

    }

}


void A2StarterAudioProcessor::getDelayBuff(juce::AudioBuffer<float>& buffer, int channel, const int numSamples,
    const int delayNumSamples, const float* bufferData, const float* delayBufferData) {
    int readPosition = (int)(delayNumSamples + WritePosition - (rate * time)) % delayNumSamples;

    if (delayNumSamples > numSamples + readPosition) {
        buffer.addFrom(channel, 0, delayBufferData + readPosition, numSamples);
    }
    else {
        const int delayRemainder = delayNumSamples - readPosition;
        buffer.addFrom(channel, 0, delayBufferData + readPosition, delayRemainder);
        buffer.addFrom(channel, delayRemainder, delayBufferData, numSamples - delayRemainder);
    }

}


void A2StarterAudioProcessor::FeedbackDelay(int channel, const int numSamples,
    const int delayNumSamples, const float* bufferData, float* data, const double gainstart, const double gainend) {



    if (delayNumSamples > numSamples + WritePosition) {
        delayBuffer.copyFromWithRamp(channel, WritePosition, data, numSamples, gainstart, gainend);

    }
    else {
        const int remainder = delayNumSamples - WritePosition;
        delayBuffer.copyFromWithRamp(channel, remainder, data, remainder, gainstart, gainend);
        delayBuffer.copyFromWithRamp(channel, 0, data, delayNumSamples - remainder, gainstart, gainend);
    }
   /* DBG("lastgain" << lastGain);
    DBG("gain" << gainstart);
    DBG("gain" << gainend);*/
}

//==============================================================================
bool A2StarterAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* A2StarterAudioProcessor::createEditor()
{
    return new A2StarterAudioProcessorEditor(*this);
}

//==============================================================================
void A2StarterAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void A2StarterAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new A2StarterAudioProcessor();
}
