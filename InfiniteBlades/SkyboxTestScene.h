#pragma once
#include "DXCore.h"
#include "ShaderManager.h"
#include "InputManager.h"
#include "MaterialManager.h"
#include "MeshManager.h"
#include "GameEntity.h"
#include "Camera.h"
#include "Lights.h"
#include "WICTextureLoader.h"
#include <vector>
#include "Skybox.h"

class SkyboxTestScene : 
	public DXCore
{
public:
	SkyboxTestScene(HINSTANCE hInstance);
	~SkyboxTestScene();
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

	// Overridden mouse input helper methods
	void OnMouseDown(WPARAM buttonState, int x, int y);
	void OnMouseUp(WPARAM buttonState, int x, int y);
	void OnMouseMove(WPARAM buttonState, int x, int y);
	void OnMouseWheel(float wheelDelta, int x, int y);

private:
	void LoaderShaderMeshMat();
	void InitInput();

	vec4 ambientLight;
	DirectionalLight directionalLight;
	PointLight pointLight;

	//objects
	GameEntity* sphere;
	Skybox* skybox;
	//Some Managers
	MaterialManager* matMngr;
	MeshManager* meshMngr;
	InputManager* inputMngr;
	ShaderManager* shaderMngr;

	//camera
	Camera* camera;
	
	POINT prevMousePos;
};

