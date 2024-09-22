#pragma once

#include "BaseShader.h"

using namespace std;
using namespace DirectX;

class TextureShader : public BaseShader
{
public:
	TextureShader(ID3D11Device* device, HWND hwnd);
	~TextureShader();

	// Pass shader parameters
	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture);

private:
	// Initialise Shader
	void initShader(const wchar_t* vs, const wchar_t* ps);

private:
	ID3D11Buffer* matrixBuffer; // Matrix Buffer to pass to the vertex shader
	ID3D11SamplerState* sampleState; 
};

