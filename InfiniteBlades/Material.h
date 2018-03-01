#pragma once
#include "Color.h"
#include "SimpleShader.h"
#include "WICTextureLoader.h"

class Material {
private:
	//texture
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;
	ID3D11ShaderResourceView* srvPtr;
	ID3D11SamplerState* samplerPtr;
public:
	Material();
	Material(SimpleVertexShader * vShader,
		SimplePixelShader* pShader,
		ID3D11Device* device,
		ID3D11DeviceContext* context,
		const wchar_t* fileName);
	~Material();

	SimpleVertexShader* GetVertexShader();
	SimplePixelShader* GetPixelShader();

	void SetVertexShader(SimpleVertexShader* newVertexShader);
	void SetPixelShader(SimplePixelShader* newPixelShader);

	void PrepareMaterial(mat4* viewMat, mat4* projMat, mat4* worldMat);
};