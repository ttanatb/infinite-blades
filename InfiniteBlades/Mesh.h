#pragma once
#include <d3d11.h>
#include "Definitions.h"
#include "Collider.h"
#include <vector>
#include <fstream>
#include <iostream>
class Mesh
{
public:
	Mesh(Vertex* vertices, int vertexCount, int* indices, int indexCount, ID3D11Device* device);
	Mesh(char* fileName, ID3D11Device * device);

	~Mesh();

	ID3D11Buffer* GetVertexBuffer();
	ID3D11Buffer* GetIndexBuffer();
	std::vector<Vertex> GetVertices();

	int GetIndexCount();
	int GetVertexCount();


private:
	void CalculateTangents(Vertex* vertices, int vertexCount, int* indices);
	void CreateBuffers(Vertex * vertices, int vertexCount, int * indices, int indexCount, ID3D11Device * device);
	ID3D11Buffer* vertexBuffer = nullptr;
	ID3D11Buffer* indexBuffer = nullptr;

	ColliderType collType;

	int indexCount = 0;
	int vertexCount = 0;

	std::vector<Vertex> vertices;

	void Release();
};