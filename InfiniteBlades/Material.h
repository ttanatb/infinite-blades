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
	bool transparentBool = false;
	float transparentStr;
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
		bool transparentBool,
		float transparentStr);
	~Material();
	//sets the vertex/pixel shader, creates texture, and creates sampler desc and state 
	void InitMaterial(SimpleVertexShader * vShader, SimplePixelShader * pShader, ID3D11Device* device, ID3D11DeviceContext* context, const wchar_t* fileName);
	SimpleVertexShader* GetVertexShader();
	SimplePixelShader* GetPixelShader();
	bool GetTransparentBool();
	void SetTransparentState(bool transparentBool);
	float GetTransparentStr();
	void SetVertexShader(SimpleVertexShader* newVertexShader);
	void SetPixelShader(SimplePixelShader* newPixelShader);

	void PrepareMaterial(mat4* worldMat);
};