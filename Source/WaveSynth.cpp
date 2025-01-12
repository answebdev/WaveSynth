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

void WaveSynth::handleMidiEvent(const juce::MidiMessage& midiEvent) {
	// The MIDI event is either the key is pressed, the key is released,
	// or there is some other type of control information.
	// Here, we will just handle MIDI key on event and MIDI key off event.
	// Check type of MIDI event passed in (key on or key ofF) -
	// Use oscillators to handle events -
	// have as many oscillators as the keys on a piano keyboard,
	// with each oscillator capable of playing an exact key (note) ('oscillatorId').
	if (midiEvent.isNoteOn()) {
		const auto oscillatorId = midiEvent.getNoteNumber();

		// Return frequency that we want to set to our oscillator
		const auto frequency = midiNoteNumberToFrequency(oscillatorId);
		oscillators[oscillatorId].setFrequency(frequency);
	}
	else if (midiEvent.isNoteOff()) {

	}
	else if (midiEvent.isAllNotesOff()) {

	}
}

float WaveSynth::midiNoteNumberToFrequency(int midiNoteNumber)
{
	// Tune instrument to be 440 Herz at A4
	constexpr auto A4_FREQUENCY = 440.f;

	// The MIDI note number for the A4 key on the MIDI keyboard is 69
	constexpr auto A4_NOTE_NUMBER = 69.f;

	// Define how many semitones in an octave
	constexpr auto SEMITONES_IN_AN_OCTAVE = 12.f;

	// Return the fundamental frequency shifted relative to its position -
	// calculate everything with respect to this A4
	return A4_FREQUENCY * std::powf(2.f, (midiNoteNumber - A4_NOTE_NUMBER) / SEMITONES_IN_AN_OCTAVE);
}
