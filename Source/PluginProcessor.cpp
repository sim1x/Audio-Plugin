/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
VolumeControlAudioProcessor::VolumeControlAudioProcessor()
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
    oversampling.reset(new dsp::Oversampling<float>(2,3, dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, false));
}

VolumeControlAudioProcessor::~VolumeControlAudioProcessor()
{
    oversampling.reset();
}

//==============================================================================
const juce::String VolumeControlAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool VolumeControlAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool VolumeControlAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool VolumeControlAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double VolumeControlAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int VolumeControlAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int VolumeControlAudioProcessor::getCurrentProgram()
{
    return 0;
}

void VolumeControlAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String VolumeControlAudioProcessor::getProgramName (int index)
{
    return {};
}

void VolumeControlAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void VolumeControlAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    oversampling->reset();
    oversampling->initProcessing(static_cast<size_t>(samplesPerBlock));
  
}

void VolumeControlAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool VolumeControlAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void VolumeControlAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
       
        //Oversampling
          juce::dsp::AudioBlock<float> inputblock(buffer);
         juce::dsp::AudioBlock<float> outputblock = oversampling->processSamplesUp(inputblock);



    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < outputblock.getNumChannels(); channel++)
    {
        //auto* channelData = buffer.getWritePointer (channel);
        for (int sample = 0; sample < outputblock.getNumSamples(); sample++)
        {
            float in = outputblock.getSample(channel, sample);
            float out =in*Drive;
            out = Volume * ((2.f / float_Pi) * atan(out));

           /*channelData[sample] = (channelData[sample] * Volume)*(((3*Drive)/2)*(1-((Drive*Drive)/3)));//Primo stadio
            channelData[sample] = (channelData[sample] * Volume) * (((3 * Drive) / 2) * (1 - ((Drive * Drive) / 3)));//Secondo stadio
            channelData[sample] = (channelData[sample] * Volume) * (((3 * Drive) / 2) * (1 - ((Drive * Drive) / 3)));//Terzo stadio
            */
            /*
           channelData[sample] *= Drive;
           channelData[sample] = Volume * ((2.f/float_Pi)*atan(channelData[sample]));
           */
            outputblock.setSample(channel, sample, out);
        }
        
        // ..do something to the data...

    }
    //Downsampling
    oversampling->processSamplesDown(inputblock);
    
}

//==============================================================================
bool VolumeControlAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* VolumeControlAudioProcessor::createEditor()
{
    return new VolumeControlAudioProcessorEditor (*this);
}

//==============================================================================
void VolumeControlAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void VolumeControlAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VolumeControlAudioProcessor();
}