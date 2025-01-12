#include "WaveSynth.h"

std::vector<float> WaveSynth::generateSineWaveTable() {
	constexpr auto WAVETABLE_LENGTH = 64;

	std::vector<float> sineWaveTable(WAVETABLE_LENGTH);

	const auto PI = std::atanf(1.f) * 4;

	for (auto i = 0; i < WAVETABLE_LENGTH; ++ i) {
		sineWaveTable[i] = std::sinf(2 * PI * static_cast<float>(i) / static_cast<float>(WAVETABLE_LENGTH));
	}
	return sineWaveTable;
}

void WaveSynth::initializeOscillators() {
	// Set number of oscillators: 128 (128 is the number of MIDI note numbers we have available.
	// We choose 128 because we want to have a polyphonic wave table synthesizer,
	// so that we can play multiple keys at once.
	constexpr auto OSCILLATORS_COUNT = 128;

	const auto waveTable = generateSineWaveTable();

	// Clear oscillators
	oscillators.clear();

	// Initialize oscillators
	for (auto i = 0; i < OSCILLATORS_COUNT; i++) {
		oscillators.emplace_back(waveTable, sampleRate);
	}
}

void WaveSynth::prepareToPlay(double sampleRate) {
	this->sampleRate = sampleRate;

	// Initialize the oscillators - every time the sampling rate changes,
	// since the oscillators also depend on the sampling rate
	initializeOscillators();
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

void WaveSynth::render(juce::AudioBuffer<float>& buffer, int startSample, int endSample) {
	// Get pointer to the first channel
	auto* firstChannel = buffer.getWritePointer(0);

	// Iterate over oscillators
	for (auto& oscillator : oscillators) {
		// Check if oscillators are active
		if (oscillator.isPlaying()) {
			// If active, give samples in a specified range
			for (auto sample = startSample; sample < endSample; ++sample) {
				// Add to the channel -
				// if we just assign each oscillator that is active,
				// this would override what is already in the channel
				firstChannel[sample] += oscillator.getSample();
			}
		}
	}

	// Do the same for all the remaining channels
	for (auto channel = 1; channel < buffer.getNumChannels(); ++channel) {
		std::copy(firstChannel + startSample, firstChannel + endSample, buffer.getWritePointer(channel) + startSample);
	}
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
		const auto oscillatorId = midiEvent.getNoteNumber();
		oscillators[oscillatorId].stop();
	}
	else if (midiEvent.isAllNotesOff()) {
		for (auto& oscillator : oscillators) {
			oscillator.stop();
		}
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
