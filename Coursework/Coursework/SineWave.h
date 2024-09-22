#pragma once
// Class for keeping all variables regarding the water simulation together

class SineWave
{
public:
	// Intialise variables
	SineWave() { totalTime = 0; amplitude = 0; frequency = 0; speed = 0; }; 

	// Setter Functions
	void setTotalTime(float newTime) { totalTime = newTime; };
	void setAmplitude(float newAmplitude) { amplitude = newAmplitude; };
	void setFrequency(float newFrequency) { frequency = newFrequency; };
	void setSpeed(float newSpeed) { speed = newSpeed;};
	// Getter Functions
	float getTotalTime() { return totalTime; };
	float getAmplitude() { return amplitude; };
	float getFrequency() { return frequency; };
	float getSpeed() { return speed; };
private:
	// Variables used to simulate body of water
	float totalTime;
	float amplitude;
	float frequency;
	float speed;
};

