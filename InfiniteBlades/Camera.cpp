#include "Camera.h"
using namespace DirectX;

void Camera::CalcDirection()
{
	XMVECTOR dirVec = XMVector3Rotate(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f),
		XMQuaternionRotationRollPitchYaw(rotX, rotY, 0.0f));
	XMStoreFloat3(&direction, dirVec);
	XMStoreFloat3(&right, XMVector3Cross(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), dirVec));
}

void Camera::CalcViewMatTransposed()
{
	//XMVECTOR posVec = XMLoadFloat3(&position);
	//XMVECTOR dirVec = XMLoadFloat3(&direction);

	XMMATRIX viewMat = XMMatrixLookToLH(
		XMLoadFloat3(&position),
		XMLoadFloat3(&direction),
		XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	XMStoreFloat4x4(&viewMatTransposed, XMMatrixTranspose(viewMat));

	shouldCalcViewMat = false;
}

void Camera::CalcProjMatTransposed()
{
	XMMATRIX P = XMMatrixPerspectiveFovLH(
		0.25f * 3.1415926535f,		// Field of View Angle
		width / height,				// Aspect ratio
		0.1f,						// Near clip plane distance
		100.0f);					// Far clip plane distance
	XMStoreFloat4x4(&projMatTransposed, XMMatrixTranspose(P)); // Transpose for HLSL!
}

Camera::Camera(float width, float height, vec3 pos, float rotX, float rotY)
{
	this->width = width;
	this->height = height;
	this->rotX = rotX;
	this->rotY = rotY;
	position = pos;
	shouldCalcViewMat = false;
	inputPtr = Input::GetInstance();

	CalcDirection();
	CalcViewMatTransposed();
	CalcProjMatTransposed();
}

void Camera::Update()
{
	if (inputPtr->GetKey('A')) 
		MoveAlongRight(-0.01f);
	if (inputPtr->GetKey('D'))
		MoveAlongRight(+0.01f);

	if (inputPtr->GetKey('W'))
		MoveAlongForward(+0.01f);
	if (inputPtr->GetKey('S'))
		MoveAlongForward(-0.01f);

	if (inputPtr->GetKey(' '))
		Move(0, +0.01f, 0);
	if (inputPtr->GetKey('X'))
		Move(0, -0.01f, 0);

	if (shouldCalcViewMat)
		CalcViewMatTransposed();
}

mat4* Camera::GetViewMatTransposed()
{
	return &viewMatTransposed;
}

mat4* Camera::GetProjMatTransposed()
{
	return &projMatTransposed;
}

vec3 Camera::GetPos()
{
	return position;
}

void Camera::Move(float x, float y, float z)
{
	XMVECTOR newPos = XMVectorAdd(XMVectorSet(x, y, z, 0.0f), XMLoadFloat3(&position));
	XMStoreFloat3(&position, newPos);
	shouldCalcViewMat = true;
}

void Camera::MoveAlongForward(float amt)
{
	XMVECTOR newPos = XMVectorAdd(XMVectorScale(XMLoadFloat3(&direction), amt), XMLoadFloat3(&position));
	XMStoreFloat3(&position, newPos);
	shouldCalcViewMat = true;
}

void Camera::MoveAlongRight(float amt)
{
	XMVECTOR newPos = XMVectorAdd(XMVectorScale(XMLoadFloat3(&right), amt), XMLoadFloat3(&position));
	XMStoreFloat3(&position, newPos);
	shouldCalcViewMat = true;
}

void Camera::RotateAroundUp(float rotationAmt)
{
	rotY += rotationAmt;
	CalcDirection();
	shouldCalcViewMat = true;
}

void Camera::RotateAroundRight(float rotationAmt)
{
	float newAmt = rotX + rotationAmt;
	if (newAmt < XM_PIDIV2 && newAmt > -XM_PIDIV2) {
		rotX = newAmt;
	} 
	CalcDirection();
	shouldCalcViewMat = true;
}

void Camera::SetWidthHeight(float width, float height)
{
	this->width = width;
	this->height = height;
	CalcProjMatTransposed();
}
