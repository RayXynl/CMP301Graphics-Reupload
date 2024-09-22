#pragma once

#include "DXF.h"
#include "SineWave.h"

using namespace std;
using namespace DirectX;

class WaterShader : public BaseShader
{
private:
	// Matrix Buffer type for passing to vertex shader
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMMATRIX lightView;
		XMMATRIX lightProjection;
	};
	// Light Buffer type for passing to pixel shader
	struct LightBufferType
	{
		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;
		XMFLOAT3 position;
		float padding;
	};
	// Helicopter Light Buffer type to pass to pixel shader
	struct hLightBufferType
	{
		XMFLOAT4 hAmbient;
		XMFLOAT4 hDiffuse;
		XMFLOAT3 hDirection;
		float hPadding;
		XMFLOAT3 hPosition;
		float hPadding_;
	};
	// Sine Wave Buffer type to pass to vertex shader
	struct SineWaveBufferType
	{
		float time;
		float amplitude;
		float frequency;
		float speed;
	};
public:
	WaterShader(ID3D11Device* device, HWND hwnd);
	~WaterShader();

	// Pass shader parameters
	void setShaderParamters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* depthMap, Light* light, Light* helicopterLights[], SineWave* sineWave);

private:
	// Initialise Shader
	void initShader(const wchar_t* vs, const wchar_t* ps);

private:
	ID3D11Buffer* matrixBuffer; // Matrix Buffer to pass to vertex shader
	ID3D11Buffer* lightBuffer; // Light Buffer to pass to pixel shader
	ID3D11Buffer* sineWaveBuffer; // Sine Wave Buffer to pass to vertex shader

	// Helicopter Light Buffer
	ID3D11Buffer* helicopterLightBuffer[3]; // Helicopter Light Buffers to pass to pixel shaders

	ID3D11BlendState* blendState; // Blend State used for making water transulcent 

	ID3D11SamplerState* sampleState;
	ID3D11SamplerState* sampleStateShadow; // Sampler state for sampling shadow map
};

