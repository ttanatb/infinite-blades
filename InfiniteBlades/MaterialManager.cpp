#include "MaterialManager.h"

MaterialManager* MaterialManager::instance = nullptr;

MaterialManager::MaterialManager() 
{
	map = std::map<char*, Material*>();
	device = nullptr;
	context = nullptr;
}

MaterialManager::~MaterialManager() 
{
	ReleaseAllMaterials();
}

MaterialManager * MaterialManager::GetInstancce()
{
	if (instance == nullptr)
		instance = new MaterialManager();

	return instance;
}

void MaterialManager::ReleaseInstance()
{
	if (instance != nullptr) {
		delete instance;
	}
}

void MaterialManager::ReleaseAllMaterials()
{
	for (const auto& pair : map) {
		delete(pair.second);
	}
}

void MaterialManager::AddMat(char * name, SimpleVertexShader * vShader, SimplePixelShader * pShader, const wchar_t * fileName)
{
	Material* mat = new Material(vShader, pShader, device, context, fileName);
	map.insert(std::pair<char*, Material*>(name, mat));
}

void MaterialManager::Init(ID3D11Device * device, ID3D11DeviceContext * context)
{
	this->device = device;
	this->context = context;
}

Material * MaterialManager::GetMat(char * name)
{
	return map[name];
}
