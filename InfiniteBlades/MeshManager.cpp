#include "MeshManager.h"
MeshManager* MeshManager::instance = nullptr;

MeshManager::MeshManager()
{
	map = std::map<char*, Mesh*>();
	device = nullptr;
}

MeshManager::~MeshManager()
{
	ReleaseAllMeshes();
}

MeshManager * MeshManager::GetInstancce()
{
	if (instance == nullptr)
		instance = new MeshManager();

	return instance;
}

void MeshManager::ReleaseInstance()
{
	if (instance != nullptr) 
		delete instance;
}

void MeshManager::ReleaseAllMeshes()
{
	for (const auto& pair : map) {
		delete(pair.second);
	}
}

void MeshManager::AddMesh(char * name, char * fileName)
{
	Mesh* mesh = new Mesh(fileName, device);
	map.insert(std::pair<char*, Mesh*>(name, mesh));
}

void MeshManager::Init(ID3D11Device * device)
{
	this->device = device;
}

Mesh * MeshManager::GetMesh(char * name)
{
	return map[name];
}
