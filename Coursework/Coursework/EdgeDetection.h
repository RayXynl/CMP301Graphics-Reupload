#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

class EdgeDetection : public BaseShader
{
private:
	struct ScreenSizeBufferType
	{
		float screenWidth;
		float screenHeight;
		XMFLOAT2 padding;
	};

	struct EdgeDetectionBufferType
	{
		float scale;
		XMFLOAT3 padding;
	};
public:
	EdgeDetection(ID3D11Device* device, HWND hwnd);
	~EdgeDetection();

	// Pass shader parameters
	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, float width, float height, float scale);
private:
	// Intialise shader
	void initShader(const wchar_t* vs, const wchar_t* ps);

private:
	ID3D11SamplerState* sampleState; // Sampler state for sampling render texture
	ID3D11Buffer* matrixBuffer; // Matrix buffer to pass to vertex shader
	ID3D11Buffer* screenSizeBuffer; // Screen size buffer to pass to pixel shader
	ID3D11Buffer* edgeDetectionBuffer; // Buffer used for user to change scale of edge detection
};

