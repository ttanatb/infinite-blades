#pragma once
#include <vector>
#include <map>
#include <algorithm>
#include <math.h>
#include <string.h>
#include "GameEntity.h"
#include "Skybox.h"
#include "Lights.h"
#include "Camera.h"
#include "DXCore.h"
#include "Reflection.h"
#include "GameManager.h"

class RenderManager
{
private:
	RenderManager();
	~RenderManager();

	static RenderManager* instance;
	typedef GameEntity* pEntity; //game entity pointer 
	unsigned int totalCount = 0; //number of objects in both list

	GameManager* gameManager;
	bool isUsingInstanced = false;

	Reflection* reflectionTex;
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	//store previous render target and DSV
	UINT numOfPreviousPorts = 1;
	D3D11_VIEWPORT previousViewport;
	ID3D11RenderTargetView* previousRenderTarget;
	ID3D11DepthStencilView* previousDSV;

	//Game Objects in the scene 
	std::vector<GameEntity*> opaqueObjects;
	std::vector<GameEntity*> transparentObjects;
	std::vector<GameEntity*> aboveGroundObjects;

	//Lights in the scene 
	vec4 ambientLight;
	std::map<char*, DirectionalLight> directionaLightMap;
	std::map<char*, PointLight> pointLightMap;

	ID3D11Device* device;
	ID3D11DeviceContext* context;

	//blend state for transparency
	ID3D11BlendState* blendState;

	//instanced rendering
	unsigned int instanceCount;
	ID3D11Buffer* instanceWorldMatrixBuffer = nullptr;
	GameEntity* gameEntityToInstance = nullptr;

	//skybox
	Skybox* skybox;

	//camera
	Camera* camera;

	//draws gameentities from a renderlist
	void DrawObjects(std::vector<GameEntity*> list, UINT stride, UINT offset, Camera* camera, mat4 ViewMatrix);
	void DrawInstanced(GameEntity* entityToInstance, Camera* camera);
	void DrawAllOpaque(Camera* cameraTarget, mat4 viewMatrix);
	void InitBlendState();
	void InitInstanceRendering(int instanceCount = 100);

	//helper functions for sorting rendering lists
	void SortObjects(std::vector<GameEntity*> list, Camera* camera);
	//reflections
	void RenderReflectionTexture();
	void ReleaseReflectionTexture();
public:
	static RenderManager* GetInstance();
	static void ReleaseInstance();
	void ReleaseBlendState();

	void Draw();

	void InitSkyBox(Skybox* skybox);
	void InitCamera(Camera* camera);
	void Init(ID3D11Device * device, ID3D11DeviceContext * context);

	void InitInstancedRendering(GameEntity* gameEntity, unsigned int instanceCount);
	void AddToTransparent(GameEntity* gameEntity);
	void AddToTransparent(std::vector<GameEntity*> entityList);
	void AddToOpqaue(GameEntity* gameEntity);
	void AddToOpqaue(std::vector<GameEntity*> entityList);
	void AddToOpaqueAndTransparent(std::vector<GameEntity*> entityList);

	void AddToReflectionRender(GameEntity* gameEntity);
	void AddToReflectionRender(std::vector<GameEntity*> entityList);
	void AddAmbientLight(vec4 ambientLight);
	void AddDirectionalLight(char* name, DirectionalLight directionalLight);
	void AddPointLight(char* name, PointLight pointLight);
};

