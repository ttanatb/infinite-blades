#pragma once
#include <map>
#include "SimpleShader.h"
class ShaderManager {
private:
	ShaderManager();
	~ShaderManager();
	static ShaderManager* instance;

	std::map<char*, ISimpleShader*>	map;

	ID3D11Device* device;
	ID3D11DeviceContext* context;
public:
	static ShaderManager* GetInstancce();
	static void ReleaseInstance();

	void ReleaseAllShaders();
	void Init(ID3D11Device* device, ID3D11DeviceContext* context);

	void AddPixelShader(char* name);
	void AddVertexShader(char* name);
	void AddDomainShader(char* name);
	void AddHullShader(char* name);
	void AddComputeShader(char* name);

	SimplePixelShader	* GetPixelShader(char* name);
	SimpleVertexShader	* GetVertexShader(char* name);
	SimpleDomainShader	* GetDomainShader(char* name);
	SimpleHullShader	* GetHullShader(char* name);
	SimpleComputeShader * GetComputeShader(char* name);

	wchar_t* ConvertName(char* name);
};