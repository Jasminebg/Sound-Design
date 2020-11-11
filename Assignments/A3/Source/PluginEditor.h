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
class A3AudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    A3AudioProcessorEditor (A3AudioProcessor&);
    ~A3AudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    A3AudioProcessor& audioProcessor;
    
    juce::Slider cutOffSlider;
    juce::Slider rateSlider;
    juce::Slider depthSlider;
    juce::Slider gainSlider;
    juce::ComboBox filterMenu;
    juce::ComboBox phaserMenu;
    juce::ComboBox impulseMenu;
    juce::ComboBox reverbType;
    //reverb
    //juce::Slider dampSlider;
    juce::Slider drySlider;
    //juce::Slider freezeSlider;
    juce::Slider roomSlider;
    juce::Slider wetSlider;
    juce::Slider widthSlider;
     
    //juce::TextButton openButton{"Open file.."};

    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> cutOffValue;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> rateValue;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> depthValue;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainValue;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> filterMenuValue;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> phaserMenuValue;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> ImpulseValue;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> reverbValue;
    //reverb
    //std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> dampValue;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> dryValue;
    //std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> freezeValue;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> roomValue;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> wetValue;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> widthValue;
    //std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> buttonValue;


    void buttonClicked();

    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (A3AudioProcessorEditor)
};
