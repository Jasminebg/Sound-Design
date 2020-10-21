/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
A2StarterAudioProcessorEditor::A2StarterAudioProcessorEditor (A2StarterAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 400);
    
    // these define the parameters of our slider object
    volumeSlider.setSliderStyle (juce::Slider::LinearBarVertical);
    volumeSlider.setRange (0.0, 2.0, 0.05);
    volumeSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 90, 0);
    volumeSlider.setPopupDisplayEnabled (true, false, this);
    volumeSlider.setTextValueSuffix (" Volume");
    volumeSlider.setValue(1.0);
 
    // this function adds the slider to the editor
    addAndMakeVisible (&volumeSlider);
    volumeSlider.addListener (this);

    // these define the parameters of our slider object
    drySlider.setSliderStyle(juce::Slider::LinearBarVertical);
    drySlider.setRange(0.0, 100.0, 1.0);
    drySlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    drySlider.setPopupDisplayEnabled(true, false, this);
    drySlider.setTextValueSuffix("% Dry");
    drySlider.setValue(100.0);

    // this function adds the slider to the editor
    addAndMakeVisible(&drySlider);
    drySlider.addListener(this);

    // these define the parameters of our slider object
    wetSlider.setSliderStyle(juce::Slider::LinearBarVertical);
    wetSlider.setRange(0.0, 100.0, 1.0);
    wetSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    wetSlider.setPopupDisplayEnabled(true, false, this);
    wetSlider.setTextValueSuffix("% Dry");
    wetSlider.setValue(100.0);

    // this function adds the slider to the editor
    addAndMakeVisible(&wetSlider);
    wetSlider.addListener(this);


    // these define the parameters of our slider object
    feedbackSlider.setSliderStyle(juce::Slider::LinearBarVertical);
    feedbackSlider.setRange(0.0, 100.0, 1.0);
    feedbackSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    feedbackSlider.setPopupDisplayEnabled(true, false, this);
    feedbackSlider.setTextValueSuffix("% Dry");
    feedbackSlider.setValue(100.0);

    // this function adds the slider to the editor
    addAndMakeVisible(&feedbackSlider);
    feedbackSlider.addListener(this);


    enum RadioButtonIDs{
        customButtons = 1001
        };
        pingpongButton.onClick = [this] { pingpongSet(&pingpongButton, pingpongButton.getButtonText());   };
        getLookAndFeel().setColour(juce::TextButton::buttonColourId, juce::Colours::pink);

        pingpongButton.setRadioGroupId(customButtons);
        addAndMakeVisible(pingpongButton);



}

A2StarterAudioProcessorEditor::~A2StarterAudioProcessorEditor()
{
}

//==============================================================================
void A2StarterAudioProcessorEditor::paint (juce::Graphics& g)
{
   // fill the whole window white
    g.fillAll (juce::Colours::white);
 
    // set the current drawing colour to black
    g.setColour (juce::Colours::black);
 
    // set the font size and draw text to the screen
    g.setFont (15.0f);
 
    g.drawFittedText ("A2 Delay", 0, 0, getWidth(), 30, juce::Justification::centred, 1);
}

void A2StarterAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    // sets the position and size of the slider with arguments (x, y, width, height)
    volumeSlider.setBounds (40, 30, 20, getHeight() - 60);
    drySlider.setBounds(volumeSlider.getX() + 40,  30, 20, getHeight() - 60);
    wetSlider.setBounds(drySlider.getX() + 40, 30, 20, getHeight() - 60);
    feedbackSlider.setBounds(wetSlider.getX() + 40, 30, 20, getHeight() - 60);
    pingpongButton.setBounds(feedbackSlider.getX() + 40, drySlider.getHeight()/2 + 30, 40, 40);
}

void A2StarterAudioProcessorEditor::sliderValueChanged (juce::Slider* slider)
{
    audioProcessor.volumeBoost = volumeSlider.getValue();
    audioProcessor.dry = drySlider.getValue();
    audioProcessor.wet= drySlider.getValue();
    audioProcessor.feedback = feedbackSlider.getValue();


}
void A2StarterAudioProcessorEditor::pingpongSet(juce::TextButton* button, juce::String state) {
    if (state == "Off") {
        audioProcessor.pingpong = true;
        button->setButtonText("On");
        button->setColour(juce::TextButton::buttonColourId, juce::Colours::black);
   }
    else {
        audioProcessor.pingpong = false;
        button->setButtonText("Off");
        button->setColour(juce::TextButton::buttonColourId, juce::Colours::pink);
    }
}

