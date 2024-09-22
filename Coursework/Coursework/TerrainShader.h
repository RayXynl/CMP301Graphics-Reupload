#pragma once

#include "DXF.h"
#include "TerrainValues.h"

using namespace std;
using namespace DirectX;

class TerrainShader : public BaseShader
{
private:
	// Matrix buffer to pass to vertex shader
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMMATRIX lightView;
		XMMATRIX lightProjection;
	};

	// Buffer for rendering the main light
	struct LightBufferType
	{
		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;
		XMFLOAT3 direction;		
		float padding;
	};

	// Buffer for editing and rendering each helicopter spotlight
	struct hLightBufferType
	{
		XMFLOAT4 hAmbient;
		XMFLOAT4 hDiffuse;
		XMFLOAT3 hDirection;
		float hPadding;
		XMFLOAT3 hPosition;
		float hPadding_;
	};

	// Buffer for editing the terrain
	struct TerrainBufferType
	{
		float frequency;
		float amplitude;
		float persistence;
		float octaves;
	};

public:
	TerrainShader(ID3D11Device* device, HWND hwnd);
	~TerrainShader();

	// Pass shader parameters
	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* depthMap, Light* light, Light* helicopterLights[], TerrainValues* terrainValues);
private:
	// Intialise shader
	void initShader(const wchar_t* vs, const wchar_t* ps);
	void initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename);
private:
	// Buffers
	ID3D11Buffer* matrixBuffer; // Matrix Buffer to pass to vertex shader
	ID3D11Buffer* lightBuffer; // Light buffer to pass to pixel shader
	ID3D11Buffer* terrainBuffer; // Terrain Buffer to pass to vertex shader

	// Helicopter Light Buffer
	ID3D11Buffer* helicopterLightBuffer[3]; // Light Buffer Array to pass to pixel shader
	
	ID3D11SamplerState* sampleState; // Sampler for texture colour
	ID3D11SamplerState* sampleStateShadow; // Sampler for applying Shadows
};

