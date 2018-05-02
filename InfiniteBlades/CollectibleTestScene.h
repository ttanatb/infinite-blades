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
#include "Player.h"
#include "Skybox.h"
#include <bitset>

class CollectibleTestScene
	: public DXCore
{

public:
	CollectibleTestScene(HINSTANCE hInstance);
	~CollectibleTestScene();

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
	bool freelookEnabled = false;

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

	//Player
	Player* player;
	Skybox* skybox;

	GameEntity* sphere1;
	GameEntity* coin1;
	GameEntity* coin2;
	GameEntity* coin3;


	// Game entities
	std::vector<GameEntity*> gameEntities;

	std::vector<std::bitset<1>> collected;

	//imgui Variables
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// Keeps track of the old mouse position.  Useful for 
	// determining how far the mouse moved in a single frame.
	POINT prevMousePos;
};

