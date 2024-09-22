#pragma once
// Class to contain all variables regarding terrain generation 
class TerrainValues
{
public:
	// Intialise variables
	TerrainValues() { frequency = 0.5; amplitude = 0.8; persistence = 0.6; octaves = 64; };
	
	// Setter Functionws
	void setFrequency(float newFrequency) { frequency = newFrequency; };
	void setAmplitude(float newAmplitude) { amplitude = newAmplitude; };
	void setPersistence(float newPersistence) { persistence = newPersistence; };
	void setOctaves(float newOctaves) { octaves = newOctaves; };
	// Getter Functions
	float getFrequency() { return frequency; };
	float getAmplitude() { return amplitude; };
	float getPersistence() { return persistence; };
	float getOctaves() { return octaves; };
private: 
	// Variables regarding terrain generation
	float frequency;
	float amplitude;
	float persistence;
	float octaves;
	
};