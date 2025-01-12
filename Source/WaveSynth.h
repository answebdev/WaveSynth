#pragma once
#include "JuceHeader.h"

class WaveSynth
{
public:
	void prepareToPlay(double sampleRate);
	void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&);

private:
	void initializeOscillators();
	std::vector<float>generateSineWaveTable();
	void handleMidiEvent(const juce::MidiMessage& midiEvent);
	float midiNoteNumberToFrequency(int midiNoteNumber);

	double sampleRate;
	std::vector<WaveTableOscillators> oscillators;
};
