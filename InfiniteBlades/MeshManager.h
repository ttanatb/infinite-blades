#pragma once
#include <map>
#include "Mesh.h"
class MeshManager {
private:
	MeshManager();
	~MeshManager();
	static MeshManager* instance;

	std::map<char*, Mesh*> map;
	ID3D11Device* device;
public:
	static MeshManager* GetInstancce();
	static void ReleaseInstance();
	void ReleaseAllMeshes();
	void AddMesh(char* name, char* fileName);
	void Init(ID3D11Device* device);
	Mesh* GetMesh(char* name);
};