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
	index = std::fmod(index, static_cast<float>(waveTable.size()));
	return sample;
}

float WaveTableOscillator::interpolateLinearly()
{
	const auto truncatedIndex = static_cast<int>(index);
	const auto nextIndex = (truncatedIndex + 1) % static_cast<int>(waveTable.size());
	const auto nextIndexWeight = index - static_cast<float>(truncatedIndex);
	const auto trucatedIndexWeight = 1.f - nextIndexWeight;

	return trucatedIndexWeight * waveTable[truncatedIndex] + nextIndexWeight * waveTable[nextIndex];

}
