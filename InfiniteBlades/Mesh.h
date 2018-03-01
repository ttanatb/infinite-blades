#pragma once
#include <d3d11.h>
#include "Vertex.h"
#include "Color.h"
#include <vector>
#include <fstream>
class Mesh
{
public:
	Mesh(Vertex* vertices, int vertexCount, int* indices, int indexCount, ID3D11Device* device);
	Mesh(char* fileName, ID3D11Device * device);
	~Mesh();

	ID3D11Buffer* GetVertexBuffer();
	ID3D11Buffer* GetIndexBuffer();
	int GetIndexCount();

	static void CreateCube(ID3D11Device* device, float size = 0.5f, Color c = Color::Red());
	static void ReleasePrimitives();
	static Mesh* GetCubeMeshPtr();
private:
	void CreateBuffers(Vertex * vertices, int vertexCount, int * indices, int indexCount, ID3D11Device * device);

	ID3D11Buffer* vertexBuffer = nullptr;
	ID3D11Buffer* indexBuffer = nullptr;
	int indexCount = 0;

	void Release();
	static Mesh* cubeMeshPtr; 
};