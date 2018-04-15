#pragma once

#include "DXCore.h"
#include "ShaderManager.h"
#include "InputManager.h"
#include "MaterialManager.h"
#include "MeshManager.h"
#include "GameEntity.h"
#include "Camera.h"
#include "Lights.h"
#include "Skybox.h"
#include <vector>

class NormalMapTestScene
	: public DXCore
{

public:
	NormalMapTestScene(HINSTANCE hInstance);
	~NormalMapTestScene();

	// Overridden setup and game loop methods, which
	// will be called automatically
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
	bool isMoving = true;

	// Initialization helper methods - feel free to customize, combine, etc.
	void LoadShaderMeshMat();
	void CreateEntities();
	void InitInput();

	int drawIndex = 0;

	vec4 ambientLight;
	DirectionalLight directionalLight;
	DirectionalLight directionalLight2;
	PointLight pointLight;

	//Some Managers
	MaterialManager* matMngr;
	MeshManager* meshMngr;
	InputManager* inputMngr;
	ShaderManager* shaderMngr;

	Skybox* skybox;

	//camera
	Camera* camera;

	// Game entities
	std::vector<GameEntity*> gameEntities;

	// Keeps track of the old mouse position.  Useful for 
	// determining how far the mouse moved in a single frame.
	POINT prevMousePos;
};

