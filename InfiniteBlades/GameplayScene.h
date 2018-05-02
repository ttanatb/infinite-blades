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
#include "Emitter.h"
#include "Player.h"
#include "Skybox.h"
#include "RenderManager.h"

class GameplayScene 
	: public DXCore
{

public:
	GameplayScene(HINSTANCE hInstance);
	~GameplayScene();

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

	// Overridden mouse input helper methods
	void OnMouseDown (WPARAM buttonState, int x, int y);
	void OnMouseUp	 (WPARAM buttonState, int x, int y);
	void OnMouseMove (WPARAM buttonState, int x, int y);
	void OnMouseWheel(float wheelDelta,   int x, int y);
private:
	bool isMoving = true;

	// Initialization helper methods - feel free to customize, combine, etc.
	void LoadShaderMeshMat();
	void CreateEntities();
	void InitInput();
	void AddEntityToRender();

	vec4 ambientLight;
	DirectionalLight directionalLight;
	DirectionalLight directionalLight2;
	PointLight pointLight;

	//Some Managers
	MaterialManager* matMngr;
	MeshManager* meshMngr;
	InputManager* inputMngr;
	ShaderManager* shaderMngr;
	RenderManager* renderMngr;

	//camera
	Camera* camera;

	//Player
	Player* player;
	Skybox* skybox;

	// Game entities
	std::vector<GameEntity*> gameEntities;
	Emitter* snowEmitter;

	// Keeps track of the old mouse position.  Useful for 
	// determining how far the mouse moved in a single frame.
	POINT prevMousePos;
};

