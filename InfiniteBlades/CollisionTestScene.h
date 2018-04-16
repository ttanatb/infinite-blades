#pragma once

#include "DXCore.h"
#include "ShaderManager.h"
#include "InputManager.h"
#include "MaterialManager.h"
#include "CollisionSolver.h"
#include "MeshManager.h"
#include "GameEntity.h"
#include "Camera.h"
#include "Lights.h"
#include "WICTextureLoader.h"
#include <vector>

class CollisionTestScene
	: public DXCore
{

public:
	CollisionTestScene(HINSTANCE hInstance);
	~CollisionTestScene();

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
	bool freelookEnabled;

	// Initialization helper methods - feel free to customize, combine, etc.
	void LoadShaderMeshMat();
	void CreateEntities();
	void InitInput();

	vec4 ambientLight;
	DirectionalLight directionalLight;
	DirectionalLight directionalLight2;
	PointLight pointLight;

	//Some Managers
	MaterialManager* matMngr;
	MeshManager* meshMngr;
	InputManager* inputMngr;
	ShaderManager* shaderMngr;

	//camera
	Camera* camera;

	// Game entities
	std::vector<GameEntity*> gameEntities;

	GameEntity* sphere1;
	GameEntity* sphere2;

	//imgui Variables
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// Keeps track of the old mouse position.  Useful for 
	// determining how far the mouse moved in a single frame.
	POINT prevMousePos;
};