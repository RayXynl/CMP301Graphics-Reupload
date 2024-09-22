#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

class LightShader : public BaseShader
{
private:
	struct LightBufferType
	{
		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;
		XMFLOAT3 position;
		float specularPower;
		XMFLOAT4 specular;
	};
public:
	LightShader(ID3D11Device* device, HWND hwnd);
	~LightShader();

	// Pass shader parameters
	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture, Light* light);

private:
	// Intialise shader
	void initShader(const wchar_t* vs, const wchar_t* ps);

private:
	ID3D11Buffer* matrixBuffer; // Matrix Buffer to pass to vertex shader
	ID3D11Buffer* lightBuffer; // Light Buffer to pass to pixel shader

	ID3D11SamplerState* sampleState;
};

