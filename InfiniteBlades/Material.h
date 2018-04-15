#pragma once
#include "Definitions.h"
#include "SimpleShader.h"
#include "WICTextureLoader.h"

class Material {
private:
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;

	bool transparentBool = false;
	float transparentStr;

	ID3D11ShaderResourceView* diffuseSRVptr;
	ID3D11SamplerState* diffuseSamplerPtr;

	ID3D11ShaderResourceView* normalSRVptr;
	ID3D11SamplerState* normalSamplerPtr;

public:
	Material(SimpleVertexShader * vShader,
		SimplePixelShader * pShader,
		ID3D11Device* device,
		ID3D11DeviceContext* context,
		const wchar_t* fileName);

	Material(SimpleVertexShader * vShader,
		SimplePixelShader* pShader,
		ID3D11Device* device,
		ID3D11DeviceContext* context,
    const wchar_t* diffuseFileName,
		bool transparentBool,
		float transparentStr);

  	Material(SimpleVertexShader * vShader,
		  SimplePixelShader* pShader,
		  ID3D11Device* device,
		  ID3D11DeviceContext* context,
      const wchar_t* diffuseFileName,
		  const wchar_t* normalFileName);

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
	void PrepareMaterialReflection(mat4* worldMat, ID3D11ShaderResourceView* skySRV);
};