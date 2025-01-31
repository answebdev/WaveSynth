#pragma once
#include <vector>

class WaveTableOscillator
{
public:
	WaveTableOscillator(std::vector<float> waveTable, double sampleRate);
	void setFrequency(float frequency);
	float getSample();
	void stop();
	bool isPlaying();
private:
	float interpolateLinearly();
	std::vector<float> waveTable;
	double sampleRate;
	float index = 0.f;
	float indexIncrement = 0.f;
};

