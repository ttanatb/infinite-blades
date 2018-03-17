#include "ShaderManager.h"

ShaderManager* ShaderManager::instance = nullptr;

ShaderManager::ShaderManager()
{
	map = std::map<char*, ISimpleShader*>();

	device = nullptr;
	context = nullptr;
}

ShaderManager::~ShaderManager()
{
	ReleaseAllShaders();
}

ShaderManager* ShaderManager::GetInstancce()
{
	if (instance == nullptr)
		instance = new ShaderManager();

	return instance;
}

void ShaderManager::ReleaseInstance()
{
	if (instance != nullptr) {
		delete instance;
	}
}

void ShaderManager::ReleaseAllShaders()
{
	for (const auto& pair : map) {
		delete(pair.second);
	}
}

void ShaderManager::Init(ID3D11Device * device, ID3D11DeviceContext * context)
{
	this->device = device;
	this->context = context;
}

void ShaderManager::AddPixelShader(char * name)
{
	SimplePixelShader * shader = new SimplePixelShader(device, context);

	wchar_t* fileName = ConvertName(name);
	shader->LoadShaderFile(fileName);
	delete[] fileName;

	map.insert(std::pair<char*, ISimpleShader*>(name, shader));
}

void ShaderManager::AddVertexShader(char * name)
{
	SimpleVertexShader * shader = new SimpleVertexShader(device, context);

	wchar_t* fileName = ConvertName(name);
	shader->LoadShaderFile(fileName); 
	delete[] fileName;

	map.insert(std::pair<char*, ISimpleShader*>(name, shader));
}

void ShaderManager::AddDomainShader(char * name)
{
	SimpleDomainShader * shader = new SimpleDomainShader(device, context);
	map.insert(std::pair<char*, ISimpleShader*>(name, shader));
}

void ShaderManager::AddHullShader(char * name)
{
	SimpleHullShader * shader = new SimpleHullShader(device, context);
	map.insert(std::pair<char*, ISimpleShader*>(name, shader));
}

void ShaderManager::AddComputeShader(char * name)
{
	SimpleComputeShader * shader = new SimpleComputeShader(device, context);
	map.insert(std::pair<char*, ISimpleShader*>(name, shader));
}

SimplePixelShader* ShaderManager::GetPixelShader(char * name)
{
	return static_cast<SimplePixelShader*>(map[name]);
}

SimpleVertexShader* ShaderManager::GetVertexShader(char * name)
{
	return static_cast<SimpleVertexShader*>(map[name]);
}

SimpleDomainShader* ShaderManager::GetDomainShader(char * name)
{
	return static_cast<SimpleDomainShader*>(map[name]);
}

SimpleHullShader* ShaderManager::GetHullShader(char * name)
{
	return static_cast<SimpleHullShader*>(map[name]);
}

SimpleComputeShader* ShaderManager::GetComputeShader(char * name)
{
	return static_cast<SimpleComputeShader*>(map[name]);
}

wchar_t * ShaderManager::ConvertName(char * name)
{
	size_t size = strlen(name) + 1;
	wchar_t* fileName = new wchar_t[size + 4];
	for (int i = 0; i < size; i++) {
		fileName[i] = name[i];
	}

	fileName[size + 3] = fileName[size - 1];
	fileName[size - 1] = L'.';
	fileName[size + 0] = L'c';
	fileName[size + 1] = L's';
	fileName[size + 2] = L'o';
	return fileName;
}

