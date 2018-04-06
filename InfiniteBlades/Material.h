#pragma once
#include "Definitions.h"
#include "SimpleShader.h"
#include "WICTextureLoader.h"

class Material {
private:
	//texture
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;
	ID3D11ShaderResourceView* srvPtr;
	ID3D11SamplerState* samplerPtr;
	D3D11_BLEND_DESC CreateTransparentDesc();
	ID3D11BlendState* transparentBlendState;
public:
	Material();
	Material(SimpleVertexShader * vShader,
		SimplePixelShader * pShader,
		ID3D11Device* device,
		ID3D11DeviceContext* context,
		const wchar_t* fileName);
	Material(SimpleVertexShader * vShader,
		SimplePixelShader* pShader,
		ID3D11Device* device,
		ID3D11DeviceContext* context,
		const wchar_t* fileName, 
		bool transparentBool);
	~Material();
	
	SimpleVertexShader* GetVertexShader();
	SimplePixelShader* GetPixelShader();
	void SetTransparentState(bool transparentBool);

	void SetVertexShader(SimpleVertexShader* newVertexShader);
	void SetPixelShader(SimplePixelShader* newPixelShader);

	void PrepareMaterial(mat4* worldMat);
};