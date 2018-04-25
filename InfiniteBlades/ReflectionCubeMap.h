#pragma once
#include <vector>
#include "GameEntity.h"
#include "Skybox.h"
#include "Lights.h"
#include "Camera.h"

class ReflectionCubeMap
{
public:
	ReflectionCubeMap(ID3D11Device* device, ID3D11DeviceContext* context, float width, float height, Skybox* skybox);
	void BuildDynamicCubeMapView();
	void BuildCubeFaceCamera(float x, float y, float z);
	void UpdateCubeFaceCamera(float x, float y, float z);
	void CalcCameras(float x, float y, float z);
	void RenderCubeMap();
	ID3D11ShaderResourceView* GetShaderResourceView();
	~ReflectionCubeMap();
private:
	const int cubeMapSize = 256;
	float width;
	float height;
	std::vector<vec3> targets;
	std::vector<vec3> ups;
	Skybox* skybox;
	ID3D11Device* device;
	ID3D11DeviceContext* context;
	D3D11_TEXTURE2D_DESC texDesc;
	ID3D11Texture2D* texture;
	ID3D11Texture2D* depthTex;
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	D3D11_TEXTURE2D_DESC depthTexDesc;
	ID3D11RenderTargetView* dynamicCubeRTV[6];
	ID3D11DepthStencilView* dynamicCubeDSV;
	ID3D11ShaderResourceView* dynamicCubeMapSRV;
	D3D11_VIEWPORT cubeMapViewPort;
	std::vector<Camera*> cubeMapCamera;
	UINT numOfPreviousPorts = 1;
	D3D11_VIEWPORT previousViewport;
	ID3D11RenderTargetView* previousRenderTarget;
	ID3D11DepthStencilView* previousDSV;
};

