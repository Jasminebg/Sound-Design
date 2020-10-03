/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
A1StarterAudioProcessor::A1StarterAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

A1StarterAudioProcessor::~A1StarterAudioProcessor()
{
}

//==============================================================================
const juce::String A1StarterAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool A1StarterAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool A1StarterAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool A1StarterAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double A1StarterAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int A1StarterAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int A1StarterAudioProcessor::getCurrentProgram()
{
    return 0;
}

void A1StarterAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String A1StarterAudioProcessor::getProgramName (int index)
{
    return {};
}

void A1StarterAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void A1StarterAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    notes.clear();                          // [1]
    currentNote = 0;                        // [2]
    lastNoteValue = -1;                     // [3]
    time = 0;                               // [4]
    rate = static_cast<float> (sampleRate); // [5]
    arpSpeed = 0.5;
    arpStyle = "Ascending";
    arpOrder = "";
    musicnotes.clear();
    musicnotes.add("C","DB","D","EB","E","F","GB","G","AB","A","BB","B" );
    alterednotes.clear();
    noteDurations.clear();
    noteDurations.add(1,1,1,1,1,1,1,1,1,1,1,1);
    octaves = 1;
    currentOctave = 0;
    currLetter = "";
    arpCustom = false;
    std::cout << arpSpeed;
    std::cout << arpStyle;
    std::cout << octaves;
    
}

void A1StarterAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool A1StarterAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void A1StarterAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
 
    // We use the audio buffer to get timing information
    auto numSamples = buffer.getNumSamples();                                                       // [7]


    for (const auto metadata : midi)                                                                // [9]
    {
        const auto msg = metadata.getMessage();

        if      (msg.isNoteOn())  notes.add (msg.getNoteNumber());
        else if (msg.isNoteOff()) notes.removeValue (msg.getNoteNumber());
    }
    alterednotes.clear();
    if(arpCustom  && arpOrder.isNotEmpty()) {
        
            juce::StringArray tokens;
            tokens.addTokens(arpOrder, " ");
            currLetter = tokens[currentNote];
           for (int i = 0; i < tokens.size() ; i++) {
                //DBG("Tokens" << tokens[i]);
                //DBG("musicnotesindex" << (musicnotes.indexOf(tokens[i])));
                //DBG("musicnotes" << musicnotes[musicnotes.indexOf(tokens[i])]);
                //DBG("Outsideinner");
                for (int j = 0; j < notes.size(); j++) {
                    //DBG("inner" << j);
                    //DBG("note " << notes[j]);
                    //DBG("note%12 " << notes[j] % 12);
                    //DBG("token " << tokens[i]);

                    if (notes[j] % 12 == (musicnotes.indexOf(tokens[i]))) {
                        alterednotes.add(notes[j]);

                    }
                }
            }
    }
    else{
        alterednotes.addArray(notes);
        
        //DBG("current " << currLetter);

        
    }


    currLetter = musicnotes[alterednotes[currentNote] % 12];
        auto noteDuration = static_cast<int> (std::ceil(rate * 0.25f * (0.1f +
            (1.0f - (arpSpeed))
            ) ));   // [8]

        if (noteDurations[alterednotes[currentNote] % 12] > 0) noteDuration = static_cast<int>
            (std::ceil(noteDuration * noteDurations[musicnotes.indexOf(currLetter)]));

  /*  DBG("Duration " << noteDuration);
    DBG("Position " << musicnotes.indexOf(currLetter));*/
 /*   DBG("C Duration " << noteDurations[0]);
    DBG("G Duration " << noteDurations[7]);*/
    midi.clear();                                                                                   // [10]


    if ((time + numSamples) >= noteDuration)                                                        // [11]
    {   // [12]

        auto offset = juce::jmax(0, juce::jmin((int)(noteDuration - time), numSamples - 1));
        if (lastNoteValue > 0)                                                                      // [13]
        {
            midi.addEvent(juce::MidiMessage::noteOff(1, lastNoteValue), offset);
            lastNoteValue = -1;
        }

        if(arpStyle == "Ascending"){ 
            
            if (alterednotes.size() > 0)                                                                       // [14]
            {
                //DBG("Letternote " << musicnotes[musicnotes.indexOf(currLetter)]);

                if ((alterednotes[currentNote] + currentOctave * 12) > 127) {
                    lastNoteValue = alterednotes[currentNote];
                }
                else lastNoteValue = (alterednotes[currentNote] + currentOctave * 12 );

                DBG("midi value " << lastNoteValue);
                 
                midi.addEvent(juce::MidiMessage::noteOn(1, lastNoteValue, (juce::uint8) 127), offset);

                currentNote = (currentNote + 1) % alterednotes.size();

            }
        }
        else if (arpStyle == "Descending")
        {

            if (alterednotes.size() > 0)                                                                       // [14]
            {
                //DBG("Letternote " << musicnotes[musicnotes.indexOf(currLetter)]);
                
                /*if (currentNote < 0) {
                    currentNote = alterednotes.size() - 1;
                }*/
                //DBG("currentNote " << currentNote);
                currentNote = (currentNote + 1) % alterednotes.size();

                if ( (alterednotes[alterednotes.size() - (1+ currentNote) ] - currentOctave * 12) < 0) {
                    lastNoteValue = alterednotes[ alterednotes.size() - (1+ currentNote) ] ;
                }
                else lastNoteValue = (alterednotes[alterednotes.size() - (1 + currentNote)] - currentOctave*12) ;
                //DBG("midi value " << lastNoteValue);

                midi.addEvent(juce::MidiMessage::noteOn(1, lastNoteValue, (juce::uint8) 127), offset);

                //currentNote -= 1;
            }
        }
        if (alterednotes.size() > 0) {

        }



       /* DBG("\n");
        DBG("currentNotenotes " << notes[currentNote]);
        DBG("Letternote" << musicnotes[alterednotes[currentNote] % 12]);
        DBG("currentvalue" << alterednotes[currentNote]);
        DBG("Midivalue " << lastNoteValue);
        DBG("Direction " << arpStyle);
        DBG("\n");*/

        if (currentNote == alterednotes.size() -1  ) {
        //if (currentNote == alterednotes.size() -1 ) {

            currentOctave = (currentOctave + 1) % octaves;
            //DBG("Octavelevel " << currentOctave);
            if (lastNoteValue + 12 > 127) {
                currentOctave = 0;
            }
            //DBG("Octaves " << octaves);
        }
        //DBG(arpOrder);

    }
    time = (time + numSamples) % noteDuration;                                                      // [15]
    }

//==============================================================================
bool A1StarterAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* A1StarterAudioProcessor::createEditor()
{
    return new A1StarterAudioProcessorEditor (*this);
}

//==============================================================================
void A1StarterAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void A1StarterAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new A1StarterAudioProcessor();
}
