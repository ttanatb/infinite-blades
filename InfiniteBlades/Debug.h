#pragma once
#include "Color.h"
#include <vector>

struct DebugLine {
	vec3 pos1;
	vec3 pos2;
};

class Debug {
private:
	Debug();
	static Debug* instance;

	std::vector<DebugLine> lines;
	std::vector<mat4> cuboids;
	std::vector<mat4> spheres;
public:
	void Initialize();
	void Release();
	void Draw();

	static void DrawLine(vec3 pos1, vec3 pos2);
	static void DrawCuboid(vec3 pos, vec3 scale);
	static void DrawSphere(vec3 pos, float radius);
};