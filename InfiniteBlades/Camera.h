#pragma once
#include "Definitions.h"
#include <iostream>
class Camera {
private:
	vec3 position;
	vec3 direction;
	vec3 right;
	float rotX;
	float rotY;

	float width;
	float height;

	mat4 viewMatTransposed;
	mat4 projMatTransposed;
	mat4 reflectionMatTranposed;

	void CalcDirection();
	void CalcViewMatTransposed();
	void CalcProjMatTransposed();

	bool shouldCalcViewMat;

public:
	Camera(float width, float height, vec3 pos, float rotX, float rotY);

	void Update();

	mat4* GetViewMatTransposed();
	mat4* GetProjMatTransposed();
	mat4* GetReflectionMat();
	void CalcReflectionMat(float height);
	vec3 GetPos();
	float GetHeight();
	float GetWidth();

	void Move(float x, float y, float z);
	void MoveAlongForward(float amt);
	void MoveAlongRight(float amt);
	void RotateAroundUp(float rotationAmt);
	void RotateAroundRight(float rotationAmt);

	void SetWidthHeight(float width, float height);
};