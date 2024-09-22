// References :
// namka. (2014). Mi-24 [3D Model]. Available at: https://www.turbosquid.com/3d-models/free-mi-24-helicopter-3d-model/863905 (Accessed: November 22, 2023).
#include "App1.h"

App1::App1()
{

}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in, bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	// Meshes Intialisation
	planeMesh = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext()); // Create plane Mesh for terrain generation
	water = new CubeMesh(renderer->getDevice(), renderer->getDeviceContext()); 	// Create cube mesh for water simulation
	orthoMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight);	// Full screen size
	lightMarker = new SphereMesh(renderer->getDevice(), renderer->getDeviceContext()); // Create light place marker mesh
	// Intialise each model and load the obj files
	helicopterOne = new AModel(renderer->getDevice(), "res/models/Mi_24.obj"); 
	helicopterTwo = new AModel(renderer->getDevice(), "res/models/Mi_24.obj");
	helicopterThree = new AModel(renderer->getDevice(), "res/models/Mi_24.obj");

	// Load Texture for helicopter model
	textureMgr->loadTexture(L"helicopter", L"res/models/textures/diffuse_m24_alpha.jpg");

	// Shaders Intialisation
	terrainShader = new TerrainShader(renderer->getDevice(), hwnd); // Intialise Terrain Shader
	waterShader = new WaterShader(renderer->getDevice(), hwnd); // Intialise Water Shader 
	lightShader = new LightShader(renderer->getDevice(), hwnd); // Intialise Light Shader 
	shadowShader = new ShadowShader(renderer->getDevice(), hwnd); // Intialise Shadow Shader
	depthShader = new DepthShader(renderer->getDevice(), hwnd); // Intialise Depth Shader
	edgeDetectionShader = new EdgeDetection(renderer->getDevice(), hwnd); // Intialise Edge Detection Shader
	textureShader = new TextureShader(renderer->getDevice(), hwnd); // Intialise Texture Shader

	// Render Texture
	terrainTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);

	// Variables for defining shadow map
	int shadowMapWidth = 2048;
	int shadowMapHeight = 2048;
	int sceneWidth = 300;
	int sceneHeight = 300;

	// Intialise Shadow Map
	shadowMap = new ShadowMap(renderer->getDevice(), shadowMapWidth, shadowMapHeight);

	// Intialise Lights
	lightOne = new Light();
	lightOne->setAmbientColour(0.5f, 0.5f, 0.5f, 1.0f);
	lightOne->setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	lightOne->setPosition(80.f, 80.f, 80.f);
	lightOne->setDirection(0.0f, -1.0f, 0.0f);
	lightOne->generateOrthoMatrix((float)sceneWidth, (float)sceneHeight, 0.1f, 100.f);

	// Intialise helicopter spotlights 
	for (int i = 0; i < 3; i++)
	{
		helicopterLights[i] = new Light();
		helicopterLights[i]->setDirection(0.0f, -1.0f, -0.5f);
	}
	// Spotlight One
	helicopterLights[0]->setAmbientColour(0.2f, 0.2f, 0.2f, 1.0f);
	helicopterLights[0]->setDiffuseColour(0.0f, 0.0f, 1.0f, 1.0f);
	helicopterLights[0]->setPosition(70.f, 40.f, 70.f);
	// Spotlight Two
	helicopterLights[1]->setAmbientColour(0.0f, 0.0f, 0.0f, 1.0f);
	helicopterLights[1]->setDiffuseColour(1.0f, 0.0f, 0.0f, 1.0f);
	helicopterLights[1]->setPosition(60.f, 40.f, 60.f);
	// Spotlight Three
	helicopterLights[2]->setAmbientColour(0.0f, 0.0f, 0.0f, 1.0f);
	helicopterLights[2]->setDiffuseColour(1.0f, 1.0f, .0f, 1.0f);
	helicopterLights[2]->setPosition(50.f, 40.f, 70.f);
	
	// Intialise SineWave Class
	sineWave = new SineWave();

	// Intialise Terrain Values Class
	terrainValues = new TerrainValues();

	// Set camera starting position
	camera->setPosition(50.f, 50.0f, -10.0f);

	// Set Water matrices 
	waterTranslation = XMMatrixTranslation(38.0f, -15.0f, 28.0f);
	waterScale = XMMatrixScaling(38.0f, 15.0f, 28.0f);
	waterMatrix = XMMatrixMultiply(waterScale, waterTranslation);

	// Set helicopter matrices 
	// Helicopter translations
	helicopterOneTranslation = XMMatrixTranslation(70.0f, 40.0f, 70.0f); 
	helicopterTwoTranslation = XMMatrixTranslation(60.f, 40.f, 60.f);
	helicopterThreeTranslation = XMMatrixTranslation(50.f, 40.f, 70.f);

	// Helicopter Scaling
	helicopterScale = XMMatrixScaling(0.05f, 0.05f, 0.05f);
	// Helicopter Rotations
	helicopterRotation = XMMatrixRotationX(-0.25f);

	// Full multiplied helicopter matrices
	helicopterOneMatrix = XMMatrixMultiply(helicopterRotation ,XMMatrixMultiply(helicopterScale, helicopterOneTranslation));
	helicopterTwoMatrix = XMMatrixMultiply(helicopterRotation ,XMMatrixMultiply(helicopterScale, helicopterTwoTranslation));
	helicopterThreeMatrix = XMMatrixMultiply(helicopterRotation ,XMMatrixMultiply(helicopterScale, helicopterThreeTranslation));

	// Terrain Scale Matrix
	terrainScale = XMMatrixScaling(1.5f, 1.0f, 1.5f);
}

App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	// Release the Direct3D object.
	if (planeMesh)
	{
		delete planeMesh;
		planeMesh = 0;
	}
	if (lightShader)
	{
		delete lightShader;
		lightShader = 0;
	}
	if (terrainShader)
	{
		delete terrainShader;
		terrainShader = 0;
	}
	if (waterShader)
	{
		delete waterShader;
		waterShader = 0;
	}
	if (shadowShader)
	{
		delete shadowShader;
		shadowShader = 0;
	}
	if (depthShader)
	{
		delete depthShader;
		depthShader = 0;
	}
}

bool App1::frame()
{
	bool result;

	result = BaseApplication::frame();
	if (!result)
	{
		return false;
	}
	
	sineWave->setTotalTime(sineWave->getTotalTime() + timer->getTime());

	// Render the graphics.
	result = render();

	if (!result)
	{
		return false;
	}

	return true;
}

bool App1::render()
{
 	//Perform first pass
	firstPass();
	// Perform depth pass
	depthPass();
	// Perform final pass
	finalPass();
	// Perfrom Edge Detection Pass
	edgeDetection();

	return true;
}

void App1::depthPass()
{
	shadowMap->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());

	lightOne->generateViewMatrix();

	XMMATRIX lightViewMatrix = lightOne->getViewMatrix();
	XMMATRIX lightProjectionMatrix = lightOne->getOrthoMatrix();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();

	// Render terrain 
	planeMesh->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), XMMatrixMultiply(worldMatrix, terrainScale), lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), planeMesh->getIndexCount());

	// Render water
	water->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), XMMatrixMultiply(worldMatrix, waterMatrix), lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), water->getIndexCount());

	// Render helicopters
	helicopterOne->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), XMMatrixMultiply(worldMatrix, helicopterOneMatrix), lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), helicopterOne->getIndexCount());

	helicopterTwo->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), XMMatrixMultiply(worldMatrix, helicopterTwoMatrix), lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), helicopterTwo->getIndexCount());

	helicopterThree->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), XMMatrixMultiply(worldMatrix, helicopterThreeMatrix), lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), helicopterThree->getIndexCount());

	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}

void App1::firstPass()
{

	terrainTexture->setRenderTarget(renderer->getDeviceContext()); // Set render target to terrain texture
	terrainTexture->clearRenderTarget(renderer->getDeviceContext(), 0.1f, 0.1f, 0.1f, 1.0f); // Clear render target to grey colour

	// Generate the view matrix based on the camera's position.
	camera->update();

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	XMMATRIX lightMarkerTranslation = XMMatrixTranslation(lightOne->getPosition().x, lightOne->getPosition().y, lightOne->getPosition().z);;

	// Render terrain
	planeMesh->sendData(renderer->getDeviceContext());
	terrainShader->setShaderParameters(renderer->getDeviceContext(), XMMatrixMultiply(worldMatrix, terrainScale), viewMatrix, projectionMatrix, shadowMap->getDepthMapSRV(), lightOne, helicopterLights, terrainValues);
	terrainShader->render(renderer->getDeviceContext(), planeMesh->getIndexCount());

	// Render light marker
	lightMarker->sendData(renderer->getDeviceContext());
	lightShader->setShaderParameters(renderer->getDeviceContext(), XMMatrixMultiply(worldMatrix, lightMarkerTranslation), viewMatrix, projectionMatrix, NULL, lightOne);
	lightShader->render(renderer->getDeviceContext(), lightMarker->getIndexCount());

	// Render water
	water->sendData(renderer->getDeviceContext());
	waterShader->setShaderParamters(renderer->getDeviceContext(), XMMatrixMultiply(worldMatrix, waterMatrix), viewMatrix, projectionMatrix, shadowMap->getDepthMapSRV(), lightOne, helicopterLights, sineWave);
	waterShader->render(renderer->getDeviceContext(), water->getIndexCount());

	// Render Helicopters
	helicopterOne->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), XMMatrixMultiply(worldMatrix, helicopterOneMatrix), viewMatrix, projectionMatrix, textureMgr->getTexture(L"helicopter"), shadowMap->getDepthMapSRV(), lightOne);
	shadowShader->render(renderer->getDeviceContext(), helicopterOne->getIndexCount());

	helicopterTwo->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), XMMatrixMultiply(worldMatrix, helicopterTwoMatrix), viewMatrix, projectionMatrix, textureMgr->getTexture(L"helicopter"), shadowMap->getDepthMapSRV(), lightOne);
	shadowShader->render(renderer->getDeviceContext(), helicopterTwo->getIndexCount());

	helicopterThree->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), XMMatrixMultiply(worldMatrix, helicopterThreeMatrix), viewMatrix, projectionMatrix, textureMgr->getTexture(L"helicopter"), shadowMap->getDepthMapSRV(), lightOne);
	shadowShader->render(renderer->getDeviceContext(), helicopterThree->getIndexCount());
	
	renderer->setBackBufferRenderTarget();
}

void App1::finalPass()
{
	// Clear the scene. (default blue colour)
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

	renderer->setZBuffer(false);

	XMMATRIX worldMatrix = renderer->getWorldMatrix(); 
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();  // ortho matrix for 2D rendering
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();	// Default camera position for orthographic rendering

	// Render Ortho Mesh
	orthoMesh->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, terrainTexture->getShaderResourceView());
	textureShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());

	renderer->setZBuffer(true);
}

void App1::edgeDetection()
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	float screenSizeX = (float)terrainTexture->getTextureWidth();
	float screenSizeY = (float)terrainTexture->getTextureHeight();

	worldMatrix = renderer->getWorldMatrix();
	baseViewMatrix = camera->getOrthoViewMatrix();
	orthoMatrix = terrainTexture->getOrthoMatrix();

	// Set Z buffer to false
	renderer->setZBuffer(false);

	// Render ortho mesh using edge detection shader
	orthoMesh->sendData(renderer->getDeviceContext());
	edgeDetectionShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, terrainTexture->getShaderResourceView(), screenSizeX, screenSizeY, scale);
	edgeDetectionShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());

	// Set Z buffer to true
	renderer->setZBuffer(true);

	renderer->setBackBufferRenderTarget();

	// Render Gui
	gui();

	// Present the rendered scene to the screen.
	renderer->endScene();
}



void App1::gui()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Build UI
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);
	
	// Edit terrain 
	float terrainFrequency = terrainValues->getFrequency();
	float terrainPersistence = terrainValues->getPersistence();
	float terrainAmplitude = terrainValues->getAmplitude();
	float terrainOctaves = terrainValues->getOctaves();

	// Terrain Sliders
	if (ImGui::CollapsingHeader("Terrain"))
	{
		ImGui::SliderFloat("tFrequency", &terrainFrequency, 0.5f, 0.8f);
		if (terrainFrequency != terrainValues->getFrequency())
		{
			terrainValues->setFrequency(terrainFrequency);
		}

		ImGui::SliderFloat("Persistence", &terrainPersistence, 0.1f, 0.7f);
		if (terrainPersistence != terrainValues->getPersistence())
		{
			terrainValues->setPersistence(terrainPersistence);
		}

		ImGui::SliderFloat("tAmplitude", &terrainAmplitude, 0.5f, 0.9f);
		if (terrainAmplitude != terrainValues->getAmplitude())
		{
			terrainValues->setAmplitude(terrainAmplitude);
		}

		ImGui::SliderFloat("Octaves", &terrainOctaves, 16, 128);
		if (terrainOctaves != terrainValues->getOctaves())
		{
			terrainValues->setOctaves(terrainOctaves);
		}
	}

	

	// Edit Waves 
	float newSpeed = sineWave->getSpeed();
	float newFreq = sineWave->getFrequency();
	float newAmplitude = sineWave->getAmplitude();

	// Water Wave Sliders
	if (ImGui::CollapsingHeader("Water"))
	{
		ImGui::SliderFloat("Speed", &newSpeed, 0.1f, 1.f);
		if (newSpeed != sineWave->getSpeed())
		{
			sineWave->setSpeed(newSpeed);
		}

		ImGui::SliderFloat("Frequency", &newFreq, 0.1f, 5.f);
		if (newFreq != sineWave->getFrequency())
		{
			sineWave->setFrequency(newFreq);
		}

		ImGui::SliderFloat("Amplitude", &newAmplitude, 0.0f, 3.f);
		if (newAmplitude != sineWave->getAmplitude())
		{
			sineWave->setAmplitude(newAmplitude);
		}

	}
	

	// Spotlights
	if (ImGui::CollapsingHeader("Helicopter Lights"))
	{
		// Spotlight One
		float spotlightOne[3] = { helicopterLights[0]->getDiffuseColour().x, helicopterLights[0]->getDiffuseColour().y, helicopterLights[0]->getDiffuseColour().z };
		ImGui::ColorEdit3("Helicopter Light One", spotlightOne);

		if (spotlightOne[0] != helicopterLights[0]->getDiffuseColour().x || spotlightOne[1] != helicopterLights[0]->getDiffuseColour().y || spotlightOne[2] != helicopterLights[0]->getDiffuseColour().z)
		{
			helicopterLights[0]->setDiffuseColour(spotlightOne[0], spotlightOne[1], spotlightOne[2], 1.0f);
		}

		// Spotlight Two
		float spotlightTwo[3] = { helicopterLights[1]->getDiffuseColour().x, helicopterLights[1]->getDiffuseColour().y, helicopterLights[1]->getDiffuseColour().z };
		ImGui::ColorEdit3("Helicopter Light Two", spotlightTwo);
		if (spotlightTwo[0] != helicopterLights[1]->getDiffuseColour().x || spotlightTwo[1] != helicopterLights[1]->getDiffuseColour().y || spotlightTwo[2] != helicopterLights[1]->getDiffuseColour().z)
		{
			helicopterLights[1]->setDiffuseColour(spotlightTwo[0], spotlightTwo[1], spotlightTwo[2], 1.0f);
		}

		// Spotlight Three
		float spotlightThree[3] = { helicopterLights[2]->getDiffuseColour().x, helicopterLights[2]->getDiffuseColour().y, helicopterLights[2]->getDiffuseColour().z };
		ImGui::ColorEdit3("Helicopter Light Three", spotlightThree);
		if (spotlightThree[0] != helicopterLights[2]->getDiffuseColour().x || spotlightThree[1] != helicopterLights[2]->getDiffuseColour().y || spotlightThree[2] != helicopterLights[2]->getDiffuseColour().z)
		{
			helicopterLights[2]->setDiffuseColour(spotlightThree[0], spotlightThree[1], spotlightThree[2], 1.0f);
		}

	}


	// Edge Detection Section
	if (ImGui::CollapsingHeader("Post Processing"))
	{
		float newScale = scale;
		ImGui::SliderFloat("Edge Scale", &newScale, 0.1f, 5.f); // Edge Detection Slider
		if (newScale != scale)
		{
			scale = newScale;
		}
	}
	
	
	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

