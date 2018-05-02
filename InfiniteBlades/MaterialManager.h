#pragma once
#include <map>
#include "Material.h"
class MaterialManager {
private:
	MaterialManager();
	~MaterialManager();
	static MaterialManager* instance;

	std::map<char*, Material*> map;
	ID3D11Device* device;
	ID3D11DeviceContext* context;
public:
	static MaterialManager* GetInstancce();
	static void ReleaseInstance();
	void ReleaseAllMaterials();

	void AddMat(char * name, 
		SimpleVertexShader * vShader, 
		SimplePixelShader * pShader, 
		const wchar_t * diffuseFileName,
		const wchar_t * normalFileName = nullptr,
		bool transparentBool = false, 
		float transparentStr = 0.5f,
		const wchar_t * reflectionFileName = nullptr);

	void Init(ID3D11Device* device, ID3D11DeviceContext* context);
	Material* GetMat(char* name);
};