#include "TerrainShader.h"

TerrainShader::TerrainShader(ID3D11Device* device, HWND hwnd) :
	BaseShader(device, hwnd)
{
	//initShader(L"terrain_vs.cso",L"terrain_hs.cso", L"terrain_ds.cso", L"terrain_ps.cso");
	initShader(L"terrain_vs.cso", L"terrain_ps.cso");
}

TerrainShader::~TerrainShader()
{
	if (sampleState)
	{
		sampleState->Release();
		sampleState = 0;
	}

	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}

	if (layout)
	{
		layout->Release();
		layout = 0;
	}

	if (lightBuffer)
	{
		lightBuffer->Release();
		lightBuffer = 0;
	}
	// Release base shader components
	BaseShader::~BaseShader();
}

void TerrainShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC hlightBufferDesc;
	D3D11_BUFFER_DESC terrainBufferDesc;

	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	renderer->CreateSamplerState(&samplerDesc, &sampleState);

	// Sampler for shadow map sampling.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.BorderColor[0] = 1.0f;
	samplerDesc.BorderColor[1] = 1.0f;
	samplerDesc.BorderColor[2] = 1.0f;
	samplerDesc.BorderColor[3] = 1.0f;
	renderer->CreateSamplerState(&samplerDesc, &sampleStateShadow);

	// Setup light buffer
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer);

	// Setup helicopter light buffer
	hlightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	hlightBufferDesc.ByteWidth = sizeof(hLightBufferType);
	hlightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hlightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	hlightBufferDesc.MiscFlags = 0;
	hlightBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&hlightBufferDesc, NULL, &helicopterLightBuffer[0]);
	renderer->CreateBuffer(&hlightBufferDesc, NULL, &helicopterLightBuffer[1]);
	renderer->CreateBuffer(&hlightBufferDesc, NULL, &helicopterLightBuffer[2]);

	// Setup terrain buffer
	terrainBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	terrainBufferDesc.ByteWidth = sizeof(TerrainBufferType);
	terrainBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	terrainBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	terrainBufferDesc.MiscFlags = 0;
	terrainBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&terrainBufferDesc, NULL, &terrainBuffer);

}

void TerrainShader::initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename)
{
	// InitShader must be overwritten and it will load both vertex and pixel shaders + setup buffers
	initShader(vsFilename, psFilename);

	// Load other required shaders.
	loadHullShader(hsFilename);
	loadDomainShader(dsFilename);
}

void TerrainShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* depthMap, Light* light, Light* helicopterLights[], TerrainValues* terrainValues)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	LightBufferType* lightPtr;
	hLightBufferType* hLightPtrOne;
	hLightBufferType* hLightPtrTwo;
	hLightBufferType* hLightPtrThree;

	XMMATRIX tworld, tview, tproj;
	// Transpose the matrices to prepare them for the shader.
	tworld = XMMatrixTranspose(worldMatrix);
	tview = XMMatrixTranspose(viewMatrix);
	tproj = XMMatrixTranspose(projectionMatrix);
	XMMATRIX tLightViewMatrix = XMMatrixTranspose(light->getViewMatrix());
	XMMATRIX tLightProjectionMatrix = XMMatrixTranspose(light->getOrthoMatrix());

	// Lock the constant buffer so it can be written to.
	deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	dataPtr->lightView = tLightViewMatrix;
	dataPtr->lightProjection = tLightProjectionMatrix;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);

	// Lights
	// Send light data to pixel shader
	deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	lightPtr = (LightBufferType*)mappedResource.pData;
	lightPtr->ambient = light->getAmbientColour();
	lightPtr->diffuse = light->getDiffuseColour();
	lightPtr->direction = light->getDirection();
	lightPtr->padding = 0.f;
	deviceContext->Unmap(lightBuffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &lightBuffer);

	// Setup 1st helicopter light to pixel shader
	deviceContext->Map(helicopterLightBuffer[0], 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	hLightPtrOne = (hLightBufferType*)mappedResource.pData;
	hLightPtrOne->hAmbient = helicopterLights[0]->getAmbientColour();
	hLightPtrOne->hDiffuse = helicopterLights[0]->getDiffuseColour();
	hLightPtrOne->hDirection = helicopterLights[0]->getDirection();
	hLightPtrOne->hPadding = 0.0f;
	hLightPtrOne->hPosition = helicopterLights[0]->getPosition();
	hLightPtrOne->hPadding_ = 0.0f;
	deviceContext->Unmap(helicopterLightBuffer[0], 0);
	deviceContext->PSSetConstantBuffers(1, 1, &helicopterLightBuffer[0]);

	// Setup 2nd helicopter light to pixel shader
	deviceContext->Map(helicopterLightBuffer[1], 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	hLightPtrTwo = (hLightBufferType*)mappedResource.pData;
	hLightPtrTwo->hAmbient = helicopterLights[1]->getAmbientColour();
	hLightPtrTwo->hDiffuse = helicopterLights[1]->getDiffuseColour();
	hLightPtrTwo->hDirection = helicopterLights[1]->getDirection();
	hLightPtrTwo->hPadding = 0.0f;
	hLightPtrTwo->hPosition = helicopterLights[1]->getPosition();
	hLightPtrTwo->hPadding_ = 0.0f;
	deviceContext->Unmap(helicopterLightBuffer[1], 0);
	deviceContext->PSSetConstantBuffers(2, 1, &helicopterLightBuffer[1]);

	// Setup 3rd helicopter light to pixel shader
	deviceContext->Map(helicopterLightBuffer[2], 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	hLightPtrThree = (hLightBufferType*)mappedResource.pData;
	hLightPtrThree->hAmbient = helicopterLights[2]->getAmbientColour();
	hLightPtrThree->hDiffuse = helicopterLights[2]->getDiffuseColour();
	hLightPtrThree->hDirection = helicopterLights[2]->getDirection();
	hLightPtrThree->hPadding = 0.0f;
	hLightPtrThree->hPosition = helicopterLights[2]->getPosition();
	hLightPtrThree->hPadding_ = 0.0f;
	deviceContext->Unmap(helicopterLightBuffer[2], 0);
	deviceContext->PSSetConstantBuffers(3, 1, &helicopterLightBuffer[2]);
	
	// Setup terrain buffer to pass to vertex shader
	TerrainBufferType* terrainPtr;
	deviceContext->Map(terrainBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	terrainPtr = (TerrainBufferType*)mappedResource.pData;
	terrainPtr->frequency = terrainValues->getFrequency();
	terrainPtr->amplitude = terrainValues->getAmplitude();
	terrainPtr->octaves = terrainValues->getOctaves();
	terrainPtr->persistence = terrainValues->getPersistence();
	deviceContext->Unmap(terrainBuffer, 0);
	deviceContext->VSSetConstantBuffers(1, 1, &terrainBuffer);

	// Shadows
	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &depthMap);
	deviceContext->PSSetSamplers(0, 1, &sampleState);
	deviceContext->PSSetSamplers(1, 1, &sampleStateShadow);
}