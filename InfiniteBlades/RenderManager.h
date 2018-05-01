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

class RenderManager
{
private:
	RenderManager();
	~RenderManager();
	static RenderManager* instance;
	typedef GameEntity* pEntity; //game entity pointer 
	unsigned int totalCount = 0; //number of objects in both list
	Skybox* skybox;
	Reflection* reflectionTex;
	Camera* camera;
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	ID3D11Device* device;
	ID3D11DeviceContext* context;
	//store previous render target and DSV
	UINT numOfPreviousPorts = 1;
	D3D11_VIEWPORT previousViewport;
	ID3D11RenderTargetView* previousRenderTarget;
	ID3D11DepthStencilView* previousDSV;
	//blend state for transparency
	ID3D11BlendState* blendState;
	//Game Objects in the scene 
	std::vector<GameEntity*> opaqueObjects;
	std::vector<GameEntity*> transparentObjects;
	std::vector<GameEntity*> aboveGroundObjects;
	//Lights in the scene 
	vec4 ambientLight;
	std::map<char*, DirectionalLight> directionaLightMap;
	std::map<char*, PointLight> pointLightMap;
	//draws gameentities from a renderlist
	void DrawObjects(std::vector<GameEntity*> list, UINT stride, UINT offset, Camera* camera, mat4 ViewMatrix); 
	void DrawAllOpaque(Camera* cameraTarget, mat4 viewMatrix);
	void InitBlendState();
	//helper functions for sorting rendering lists
	void SortOpqaue(std::vector<GameEntity*> list, Camera* camera);
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
	void AddToTransparent(GameEntity* gameEntity);
	void AddToOpqaue(GameEntity* gameEntity);
	void AddAmbientLight(vec4 ambientLight);
	void AddDirectionalLight(char* name, DirectionalLight directionalLight);
	void AddPointLight(char* name, PointLight pointLight);
};

