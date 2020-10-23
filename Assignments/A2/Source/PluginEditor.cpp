/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
A2StarterAudioProcessorEditor::A2StarterAudioProcessorEditor(A2StarterAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(500, 500);

    // these define the parameters of our slider object
    timeSlider.setSliderStyle(juce::Slider::LinearBarVertical);
    timeSlider.setRange(0.0, 3.0, 0.05);
    timeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    timeSlider.setPopupDisplayEnabled(true, false, this);
    timeSlider.setTextValueSuffix(" Time (seconds)");
    timeSlider.setValue(0);

    //// this function adds the slider to the editor
    addAndMakeVisible(&timeSlider);
    timeSlider.addListener(this);

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
    wetSlider.setTextValueSuffix("% Wet");
    wetSlider.setValue(0.0);

    // this function adds the slider to the editor
    addAndMakeVisible(&wetSlider);
    wetSlider.addListener(this);


    // these define the parameters of our slider object
    feedbackSlider.setSliderStyle(juce::Slider::LinearBarVertical);
    feedbackSlider.setRange(0.0, 100.0, 1.0);
    feedbackSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    feedbackSlider.setPopupDisplayEnabled(true, false, this);
    feedbackSlider.setTextValueSuffix("% Feedback");
    feedbackSlider.setValue(0.0);

    // this function adds the slider to the editor
    addAndMakeVisible(&feedbackSlider);
    feedbackSlider.addListener(this);


    enum RadioButtonIDs {
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
void A2StarterAudioProcessorEditor::paint(juce::Graphics& g)
{
    // fill the whole window white
    g.fillAll(juce::Colours::white);

    // set the current drawing colour to black
    g.setColour(juce::Colours::black);

    // set the font size and draw text to the screen
    g.setFont(15.0f);

    g.drawFittedText("A2 Delay", 0, 0, getWidth(), 30, juce::Justification::centred, 1);
    g.drawFittedText("Time interval", timeSlider.getX()-20, timeSlider.getY() - 30, 80, 30, juce::Justification::centredLeft, 1);
    g.drawFittedText("Dry", drySlider.getX(), drySlider.getY()-30, 80, 30, juce::Justification::centredLeft, 1);
    g.drawFittedText("Wet", wetSlider.getX(), wetSlider.getY()-30, 80, 30, juce::Justification::centredLeft, 1);
    g.drawFittedText("Feedback", feedbackSlider.getX()-20, feedbackSlider.getY() - 30, 80, 30, juce::Justification::centredLeft, 1);
    g.drawFittedText("PingPong", pingpongButton.getX()-20, pingpongButton.getY() - 30, 80, 30, juce::Justification::centredLeft, 1);
}

void A2StarterAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    // sets the position and size of the slider with arguments (x, y, width, height)
    timeSlider.setBounds(40, 50, 20, getHeight() - 60);
    drySlider.setBounds(timeSlider.getX() + 80, 50, 20, getHeight() - 60);
    wetSlider.setBounds(drySlider.getX() + 80, 50, 20, getHeight() - 60);
    feedbackSlider.setBounds(wetSlider.getX() + 80, 50, 20, getHeight() - 60);
    pingpongButton.setBounds(feedbackSlider.getX() + 80, drySlider.getHeight() / 2 + 30, 40, 40);
}

void A2StarterAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    audioProcessor.time = timeSlider.getValue();
    audioProcessor.dry = drySlider.getValue();
    audioProcessor.wet = wetSlider.getValue();
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

