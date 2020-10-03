/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class A1StarterAudioProcessorEditor  : public juce::AudioProcessorEditor, private juce::Slider::Listener//, private juce::Label::Listener
{
public:
    A1StarterAudioProcessorEditor (A1StarterAudioProcessor&);
    ~A1StarterAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void sliderValueChanged (juce::Slider* slider);
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    A1StarterAudioProcessor& audioProcessor;

    juce::Slider arpSlider; // [1]
    juce::Slider octaveSlider;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (A1StarterAudioProcessorEditor)



private:
        void arpeggiationStyle(juce::TextButton* toggles, juce::String style) ;

        juce::TextButton ascButton{ "Ascending" }, descButton{ "Descending" }, customButton{ "Custom" };

private:
        void arpeggiationOrder(juce::TextEditor* input);
        juce::TextEditor noteOrder;

private:
        void noteDurations(juce::TextEditor* text, int pos);
        juce::TextEditor noteDurationA, noteDurationB, noteDurationC, noteDurationD,
                        noteDurationE, noteDurationF, noteDurationG,
                        noteDurationAb, noteDurationBb, noteDurationDb,
                        noteDurationEb, noteDurationGb;


};
