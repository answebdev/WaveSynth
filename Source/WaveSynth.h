#pragma once
#include "JuceHeader.h"

class WaveSynth
{
public:
	void prepareToPlay(double sampleRate);
	void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&);

private:
	double sampleRate;
};

