// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "DXF.h"
// Shaders
#include "LightShader.h"
#include "TerrainShader.h"
#include "WaterShader.h"
#include "ShadowShader.h"
#include "DepthShader.h"
#include "EdgeDetection.h"
#include "TextureShader.h"
// Other includes
#include "SineWave.h"
#include "TerrainValues.h"

class App1 : public BaseApplication
{
public:
	App1();
	~App1();
	//Scene initialisation
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);

	bool frame();

protected:
	bool render();

	void depthPass(); // Depth Pass for shadow Rendering
	void firstPass(); // Renderers first pass
	void finalPass(); // Renderers final pass
	void edgeDetection(); // Edge detection pass
	void gui();

private:
	// Shaders
	TerrainShader* terrainShader;
	WaterShader* waterShader;
	ShadowShader* shadowShader;
	DepthShader* depthShader;
	LightShader* lightShader;
	EdgeDetection* edgeDetectionShader;
	TextureShader* textureShader;
	// Shadow Map
	ShadowMap* shadowMap;

	// Lights
	Light* lightOne; // Main Light

	Light* helicopterLights[3]; // Helicopter point lights

	// Meshes
	PlaneMesh* planeMesh; // Plane Mesh used for terrain generation
	CubeMesh* water; 	// Cube Mesh used for water simulation
	SphereMesh* lightMarker; 	// Sphere Mesh to show location of lightOne
	OrthoMesh* orthoMesh; // Ortho Mesh used for render texture and post processing application

	// Models
	AModel* helicopterOne; // Helicopter model
	AModel* helicopterTwo;
	AModel* helicopterThree;

	// Matrices for body of water
	XMMATRIX waterTranslation;
	XMMATRIX waterScale;
	XMMATRIX waterMatrix;

	// Translation matrices for helicopter - individual matrices
	XMMATRIX helicopterOneTranslation;
	XMMATRIX helicopterTwoTranslation;
	XMMATRIX helicopterThreeTranslation;

	// Scale and Rotation matrices for helicopters - applied to all 
	XMMATRIX helicopterScale;
	XMMATRIX helicopterRotation;

	// Helicopter Full multiplied matrices
	XMMATRIX helicopterOneMatrix;
	XMMATRIX helicopterTwoMatrix;
	XMMATRIX helicopterThreeMatrix;

	// Terrain Matrix
	XMMATRIX terrainScale;

	// SineWave class stores all data for water simulation
	SineWave* sineWave;
	TerrainValues* terrainValues;

	float scale;

	// Render Texture 
	RenderTexture* terrainTexture;
};

#endif