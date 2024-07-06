/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
VolumeControlAudioProcessorEditor::VolumeControlAudioProcessorEditor (VolumeControlAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    Volume.setSliderStyle(Slider::SliderStyle::RotaryHorizontalDrag);
    Volume.setTextBoxStyle(Slider::TextBoxBelow, true, 50, 20);
    Volume.setRange(0.0f, 1.0f, 0.01f);
    Volume.setValue(0.5f);
    Volume.addListener(this);
    addAndMakeVisible(Volume);

    Drive.setSliderStyle(Slider::SliderStyle::RotaryHorizontalDrag);
    Drive.setTextBoxStyle(Slider::TextBoxBelow, true, 50, 20);
    Drive.setRange(0.0f, 3000.f, 0.01f);
    Drive.setValue(0.5f);
    Drive.addListener(this);
    addAndMakeVisible(Drive);







   setSize (400, 300);
}

VolumeControlAudioProcessorEditor::~VolumeControlAudioProcessorEditor()
{
}

//==============================================================================
void VolumeControlAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(Colours::green);
    g.drawText("Volume", getWidth() / 4 - 100, getHeight() / 2 - 125,200,250, Justification::centred,false);
    g.drawText("Drive", getWidth() / 1.5 - 65, getHeight() / 2 - 125, 200, 250, Justification::centred, false);
   
}

void VolumeControlAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    Volume.setBounds(getWidth()/4 -100, getHeight()/2- 125, 200, 250);
   
    
    Drive.setBounds(getWidth()/1.5-65, getHeight()/2-125, 200, 250);
   
   
}
void VolumeControlAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
    if (slider == &Volume)
    {
        audioProcessor.Volume = Volume.getValue();
    }

    if (slider == &Drive)
    {
        audioProcessor.Drive = Drive.getValue();
    }


}
