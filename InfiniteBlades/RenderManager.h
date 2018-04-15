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
	//skybox
	Skybox* skybox;
	//camera
	Camera* camera;
	//draws gameentities from a renderlist
	void DrawObjects(std::vector<GameEntity*> list, UINT stride, UINT offset, Camera* camera); 
	void InitBlendState();
	//helper functions for sorting rendering lists
	void SortOpqaue(std::vector<GameEntity*> list, Camera* camera);
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
	void AddAmbientLight(vec4 ambientLight);
	void AddDirectionalLight(char* name, DirectionalLight directionalLight);
	void AddPointLight(char* name, PointLight pointLight);
};

