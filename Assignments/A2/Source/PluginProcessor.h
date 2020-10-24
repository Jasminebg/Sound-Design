/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class A2StarterAudioProcessor : public juce::AudioProcessor
{
public:
    //float volumeBoost;
    double dry, wet, feedback, time, lastGain, lastFeed;
    bool pingpong;
    int lastping;


    //==============================================================================
    A2StarterAudioProcessor();
    ~A2StarterAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;
    void fillDelayBuff(int channel, const int numSamples, const int delayBufferLen,
        const float* bufferData, const float* delayBufferData, const double gainstart, const double gainend);
    void getDelayBuff(juce::AudioBuffer<float>& buffer, int channel, const int numSamples,
        const int delayBufferLen, const float* bufferData, const float* delayBufferData);
    void FeedbackDelay( int channel, const int numSamples, const int delayNumSamples,  float* data,
                        const double gainstart, const double gainend);
private:

    float rate;
    juce::AudioBuffer<float> delayBuffer;
    int delayBufferLength;
    int delayReadPosition;
    int WritePosition { 0 } ;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(A2StarterAudioProcessor)
};
