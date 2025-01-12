#include "WaveTableOscillator.h"
#include <cmath>

WaveTableOscillator::WaveTableOscillator(std::vector<float> waveTable, 
	double sampleRate)
	: waveTable{std::move(waveTable)},
	sampleRate{ sampleRate }
{

}

void WaveTableOscillator::setFrequency(float fequency) {
	indexIncrement = frequency * static_cast<float>(waveTable.size())
		/ static_cast<float>(sampleRate);
}

float WaveTableOscillator::getSample() {
	const auto sample = interpolateLinearly();
	index += indexIncrement;
	index = std::fmod(index, waveTable.size());
	return sample;
}