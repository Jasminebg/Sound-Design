/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
A1StarterAudioProcessorEditor::A1StarterAudioProcessorEditor (A1StarterAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (600, 400);
    
    // these define the parameters of our slider object
    arpSlider.setSliderStyle (juce::Slider::LinearBarVertical);
    arpSlider.setRange (0.0, 1.0, 0.05);
    arpSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 90, 0);
    arpSlider.setPopupDisplayEnabled (true, false, this);
    arpSlider.setTextValueSuffix (" Speed");
    arpSlider.setValue(0.5);
 
    // this function adds the slider to the editor
    addAndMakeVisible (&arpSlider);
    arpSlider.addListener (this);


    octaveSlider.setSliderStyle(juce::Slider::LinearBarVertical);
    octaveSlider.setRange(1, 10, 1);
    octaveSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    octaveSlider.setPopupDisplayEnabled(true, false, this);
    octaveSlider.setTextValueSuffix(" Octaves");
    octaveSlider.setValue(1);

    addAndMakeVisible(&octaveSlider);
    octaveSlider.addListener(this);

     enum RadioButtonIds
     {
         arpButtons = 1001,
        customButtons = 1002
     };
         ascButton.onClick = [this] { arpeggiationStyle (&ascButton,   "Ascending");   };
         getLookAndFeel().setColour(juce::TextButton::buttonColourId, juce::Colours::pink);
         descButton.onClick = [this] { arpeggiationStyle (&descButton, "Descending"); };
         customButton.onClick = [this] { arpeggiationStyle(&customButton, "Custom"); };
         ascButton.setColour(juce::TextButton::buttonColourId, juce::Colours::black);

         ascButton.setRadioGroupId (customButtons);
         descButton.setRadioGroupId (arpButtons);
         customButton.setRadioGroupId(arpButtons);
         addAndMakeVisible(customButton);
         addAndMakeVisible(descButton);
         addAndMakeVisible(ascButton);


         
         noteOrder.onTextChange = [this] {arpeggiationOrder(&noteOrder); };
         addAndMakeVisible(noteOrder);

        //could not get array of TextEditors working

        noteDurationC.setText("1");
        noteDurationDb.setText("1");
        noteDurationD.setText("1");
        noteDurationEb.setText("1");
        noteDurationE.setText("1");
        noteDurationF.setText("1");
        noteDurationGb.setText("1");
        noteDurationG.setText("1");
        noteDurationAb.setText("1");
        noteDurationA.setText("1");
        noteDurationBb.setText("1");
        noteDurationB.setText("1");

        addAndMakeVisible(noteDurationC);
        addAndMakeVisible(noteDurationDb);
        addAndMakeVisible(noteDurationD);
        addAndMakeVisible(noteDurationEb);
        addAndMakeVisible(noteDurationE);
        addAndMakeVisible(noteDurationF);
        addAndMakeVisible(noteDurationGb);
        addAndMakeVisible(noteDurationG);
        addAndMakeVisible(noteDurationAb);
        addAndMakeVisible(noteDurationA);
        addAndMakeVisible(noteDurationBb);
        addAndMakeVisible(noteDurationB);

        noteDurationC.onTextChange = [this] { noteDurations(&noteDurationC, 0); };
        noteDurationDb.onTextChange = [this] { noteDurations(&noteDurationDb, 1); };
        noteDurationD.onTextChange = [this] { noteDurations(&noteDurationD, 2); };
        noteDurationEb.onTextChange = [this] { noteDurations(&noteDurationEb, 3); };
        noteDurationE.onTextChange = [this] { noteDurations(&noteDurationE, 4); };
        noteDurationF.onTextChange = [this] { noteDurations(&noteDurationF, 5); };
        noteDurationGb.onTextChange = [this] { noteDurations(&noteDurationGb, 6); };
        noteDurationG.onTextChange = [this] { noteDurations(&noteDurationG, 7); };
        noteDurationAb.onTextChange = [this] { noteDurations(&noteDurationAb, 8); };
        noteDurationA.onTextChange = [this] { noteDurations(&noteDurationA, 9); };
        noteDurationBb.onTextChange = [this] { noteDurations(&noteDurationBb, 10); };
        noteDurationB.onTextChange = [this] { noteDurations(&noteDurationB, 11); };



}

A1StarterAudioProcessorEditor::~A1StarterAudioProcessorEditor()
{
}

//==============================================================================
void A1StarterAudioProcessorEditor::paint (juce::Graphics& g)
{
   // fill the whole window white
    g.fillAll (juce::Colours::white);
 
    // set the current drawing colour to black
    g.setColour (juce::Colours::black);
 
    // set the font size and draw text to the screen
    g.setFont (15.0f);
 
    g.drawFittedText ("Arpeggiator", 0, 0, getWidth(), 30, juce::Justification::centred, 1);

    g.drawFittedText("Octaves: ", octaveSlider.getX() - 70, octaveSlider.getY() + octaveSlider.getHeight()/3, getWidth(), 30, juce::Justification::centredLeft, 1);

    g.drawFittedText("Arpeggiator Direction: ",ascButton.getX(), ascButton.getY() - 30, getWidth(), 30, juce::Justification::centredLeft, 1);

    g.drawFittedText ("Custom Note Order \n (Include spaces between notes,i.e.\"Db A Gb C \"): ", 
                        noteOrder.getX() - 75, noteOrder.getY()-30, getWidth(), 30, juce::Justification::centredLeft, 1);


    g.drawFittedText("Custom note order toggle: ", customButton.getX(), customButton.getY() - 30, getWidth(), 30, 
                            juce::Justification::centredLeft, 1);

    g.drawFittedText ("Note Durations ( 0 < duration <= 2 ): ", 
                        noteDurationF.getX()-50, noteDurationF.getY()-40, getWidth(), 30, juce::Justification::centredLeft, 1);
    for(int i = 0; i < 12; i ++){
        g.drawFittedText (audioProcessor.musicnotes[i], 
                        noteDurationC.getX()+ 10 +((i)*30), noteDurationC.getY() - 20, getWidth(),
                        30, juce::Justification::centredLeft, 1);
        }


}

void A1StarterAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    // sets the position and size of the slider with arguments (x, y, width, height)
    arpSlider.setBounds (40, 30, 20, getHeight() - 60);
    ascButton.setBounds((arpSlider.getScreenX() + 45), (arpSlider.getScreenY() + 30), 100, 40);
 //   ascButton.changeWidthToFitText();
    descButton.setBounds(ascButton.getX()+100, ascButton.getY() , 100, 40);
   // descButton.changeWidthToFitText();
    customButton.setBounds(ascButton.getX(), descButton.getY()+100, 100, 40);
    // customButton.changeWidthToFitText();
    noteOrder.setBounds(getWidth() / 2 - 50, getHeight() / 2 + 40, 100, 20);
    octaveSlider.setBounds(getWidth()-65 , 30 , 20, getHeight() - 60);

    noteDurationC.setBounds(getWidth()/4 - 25, getHeight()/2+100, 30, 20);
    noteDurationDb.setBounds(noteDurationC.getX()+30, noteDurationC.getY(), 30, 20);
    noteDurationD.setBounds(noteDurationDb.getX() + 30, noteDurationC.getY(), 30, 20);
    noteDurationEb.setBounds(noteDurationD.getX() + 30, noteDurationC.getY(), 30, 20);
    noteDurationE.setBounds(noteDurationEb.getX() + 30, noteDurationC.getY(), 30, 20);
    noteDurationF.setBounds(noteDurationE.getX() + 30, noteDurationC.getY(), 30, 20);
    noteDurationGb.setBounds(noteDurationF.getX() + 30, noteDurationC.getY(), 30, 20);
    noteDurationG.setBounds(noteDurationGb.getX() + 30, noteDurationC.getY(), 30, 20);
    noteDurationAb.setBounds(noteDurationG.getX() + 30, noteDurationC.getY(), 30, 20);
    noteDurationA.setBounds(noteDurationAb.getX() + 30, noteDurationC.getY(), 30, 20);
    noteDurationBb.setBounds(noteDurationA.getX() + 30, noteDurationC.getY(), 30, 20);
    noteDurationB.setBounds(noteDurationBb.getX() + 30, noteDurationC.getY(), 30, 20);
}

void A1StarterAudioProcessorEditor::sliderValueChanged (juce::Slider* slider)
{
    audioProcessor.arpSpeed = arpSlider.getValue();
    audioProcessor.octaves = octaveSlider.getValue();
}

void A1StarterAudioProcessorEditor::arpeggiationStyle(juce::TextButton* toggles, juce::String style)
{
    if(style == "Ascending"){
        audioProcessor.arpStyle = style;
        ascButton.setColour(juce::TextButton::buttonColourId, juce::Colours::black);
        descButton.setColour(juce::TextButton::buttonColourId, juce::Colours::pink);
    }
    else if (style == "Descending") {
        audioProcessor.arpStyle = style;
        ascButton.setColour(juce::TextButton::buttonColourId, juce::Colours::pink);
        descButton.setColour(juce::TextButton::buttonColourId, juce::Colours::black);
    }
    else if (style == "Custom") {
        if (audioProcessor.arpCustom) {
            audioProcessor.arpCustom = false;
            customButton.setColour(juce::TextButton::buttonColourId, juce::Colours::pink);
        }
        else {
            audioProcessor.arpCustom = true;
            customButton.setColour(juce::TextButton::buttonColourId, juce::Colours::black);
        }
    }
}

void A1StarterAudioProcessorEditor::arpeggiationOrder(juce::TextEditor* input) {
    
        juce::String playingorder = "";

        juce::StringArray tokens;
        tokens.addTokens(input->getText(), " ");
        for (int i = 0; i < tokens.size() - 1; i++) {
            if(!audioProcessor.musicnotes.contains(tokens[i].toUpperCase())){


            }
            else {
                playingorder = playingorder + tokens[i].toUpperCase() + " ";

            }
        

        }
        audioProcessor.arpOrder = playingorder.toUpperCase();
    

}
void A1StarterAudioProcessorEditor::noteDurations(juce::TextEditor* text, int pos) {

    if (text->getText().getDoubleValue() > 0 && text->getText().getDoubleValue()<= 2) {
        audioProcessor.noteDurations.insert(pos, text->getText().getDoubleValue());
    }

    }
