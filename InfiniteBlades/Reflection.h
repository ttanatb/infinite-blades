#pragma once
#include <vector>
#include "GameEntity.h"
#include "Skybox.h"
#include "Lights.h"
#include "Camera.h"


class Reflection
{
public:
	Reflection(ID3D11Device* device, ID3D11DeviceContext* context, int height, int width);
	void Init();
	void SetRenderTarget();
	void ClearRenderTarget(float color[4]);
	ID3D11RenderTargetView* GetRTV();
	ID3D11ShaderResourceView* GetShaderResourceView();
	void Release();
	~Reflection();
private:
	int height;
	int width;
	ID3D11Device* device;
	ID3D11DeviceContext* context;
	ID3D11Texture2D* texture;
	ID3D11Texture2D* depthTex;
	D3D11_TEXTURE2D_DESC texDesc;
	D3D11_TEXTURE2D_DESC depthTexDesc;
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ID3D11RenderTargetView* dynamicTexRTV;
	ID3D11ShaderResourceView* dynamicTexSRV;
	ID3D11DepthStencilView* dynamicTexDSV;
};

