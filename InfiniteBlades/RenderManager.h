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

class RenderManager
{
private:
	RenderManager();
	~RenderManager();

	static RenderManager* instance;
	typedef GameEntity* pEntity; //game entity pointer 
	unsigned int totalCount = 0; //number of objects in both list

	//Game Objects in the scene 
	std::vector<GameEntity*> opaqueObjects;
	std::vector<GameEntity*> transparentObjects;

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
	DirectX::XMFLOAT4X4* instanceWorldMatrices = nullptr;
	ID3D11Buffer* instanceWorldMatrixBuffer = nullptr;
	GameEntity* gameEntityToInstance = nullptr;

	//skybox
	Skybox* skybox;

	//camera
	Camera* camera;

	//draws gameentities from a renderlist
	void DrawObjects(std::vector<GameEntity*> list, UINT stride, UINT offset, Camera* camera);
	void DrawInstanced(GameEntity* entityToInstance, Camera* camera);
	void InitBlendState();
	void InitInstanceRendering(int instanceCount = 100);

	//helper functions for sorting rendering lists
	void SortObjects(std::vector<GameEntity*> list, Camera* camera);
public:
	static RenderManager* GetInstance();
	static void ReleaseInstance();
	void ReleaseBlendState();

	void Draw();

	void InitSkyBox(Skybox* skybox);
	void InitCamera(Camera* camera);
	void Init(ID3D11Device * device, ID3D11DeviceContext * context);

	void AddToTransparent(GameEntity* gameEntity);
	void AddToOpqaue(GameEntity* gameEntity);
	void InitInstancedRendering(GameEntity* gameEntity, unsigned int instanceCount);
	void AddAmbientLight(vec4 ambientLight);
	void AddDirectionalLight(char* name, DirectionalLight directionalLight);
	void AddPointLight(char* name, PointLight pointLight);
};

