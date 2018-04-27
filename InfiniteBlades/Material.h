#pragma once
#include "Definitions.h"
#include "SimpleShader.h"
#include "WICTextureLoader.h"
#include "DDSTextureLoader.h"

class Material {
private:
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;

	bool transparentBool;// = false;
	float transparentStr;

	ID3D11ShaderResourceView* reflectionSRVptr;
	ID3D11ShaderResourceView* reflectionPlanarSRVptr;
	ID3D11SamplerState* reflectionSamplerPtr;

	ID3D11ShaderResourceView* diffuseSRVptr;
	ID3D11SamplerState* diffuseSamplerPtr;

	ID3D11ShaderResourceView* normalSRVptr;
	ID3D11SamplerState* normalSamplerPtr;

public:
  	Material(SimpleVertexShader * vShader,
		SimplePixelShader* pShader,
		ID3D11Device* device,
		ID3D11DeviceContext* context,
		const wchar_t* diffuseFileName,
		const wchar_t* normalFileName = nullptr,
		bool transparentBool = false,
		float transparentStr = 0.5f,
		const wchar_t* reflectionFileName = nullptr);

	~Material();
	//sets the vertex/pixel shader, creates texture, and creates sampler desc and state 
	void InitMaterial(SimpleVertexShader * vShader, 
		SimplePixelShader * pShader, 
		ID3D11Device * device, 
		ID3D11DeviceContext * context, 
		const wchar_t * diffuseFileName, 
		const wchar_t * normalFileName,
		const wchar_t * reflectionFileName);
	SimpleVertexShader* GetVertexShader();
	SimplePixelShader* GetPixelShader();
	bool GetTransparentBool();
	void SetTransparentState(bool transparentBool);
	float GetTransparentStr();
	void SetReflectionFile(const wchar_t * reflectionFileName);
	void SetReflectionSRV(ID3D11ShaderResourceView* srv);
	void SetVertexShader(SimpleVertexShader* newVertexShader);
	void SetPixelShader(SimplePixelShader* newPixelShader);

	void PrepareMaterial(mat4* worldMat);
	void PreparePlanarReflectionMaterial(mat4* worldMat, mat4* viewMat);
	
};