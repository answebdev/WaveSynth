#include "WaveSynth.h"

void WaveSynth::prepareToPlay(double sampleRate) {
	this->sampleRate = sampleRate;
}

void WaveSynth::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
	auto currentSample = 0;

	// Iterate over MIDI buffer
	for (const auto midiMessage : midiMessages) {
		const auto midiEvent = midiMessage.getMessage();
		const auto midiEventSample = static_cast<int>(midiEvent.getTimeStamp());

		render(buffer, currentSample, midiEventSample);

		// Handle the MIDI event after rendering the sound
		handleMidiEvent(midiEvent);

		// Update current sample to be at the index of the MIDI event
		currentSample = midiEventSample;
	}

	// After all MIDI events have been handled, render last portion of the audio buffer
	render(buffer, currentSample, buffer.getNumSamples());
}