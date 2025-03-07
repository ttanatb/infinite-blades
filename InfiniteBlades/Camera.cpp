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

void Camera::CalcReflectionMat(float height)
{
	float offset = 10.0f;
	vec3 up, pos, lookAt;
	float radians;
	up = vec3(0.0f, 1.0f, 0.0f);
	pos = vec3(position.x + offset, -position.y + (height * 2.0f), position.z + offset);
	//cakc the rotation in radians
	//radians = rotY * 0.0174532925f;
	radians = rotY * (XM_PI / 180.0f);
	// Setup where the camera is looking.
	lookAt.x = sinf(radians) + position.x;
	lookAt.y = position.y;
	lookAt.z = cosf(radians) + position.z;
	// Create the view matrix from the three vectors.
	XMMATRIX reflMatrix = XMMatrixLookAtLH(XMLoadFloat3(&pos), XMLoadFloat3(&lookAt), XMLoadFloat3(&up));
	XMStoreFloat4x4(&reflectionMatTranposed, XMMatrixTranspose(reflMatrix));
}

Camera::Camera(float width, float height, vec3 pos, float rotX, float rotY)
{
	this->width = width;
	this->height = height;
	this->rotX = rotX;
	this->rotY = rotY;
	position = pos;
	shouldCalcViewMat = false;

	CalcDirection();
	CalcViewMatTransposed();
	CalcProjMatTransposed();
}

void Camera::Update()
{
	//if (inputPtr->GetKey('A')) 
	//	MoveAlongRight(-0.01f);
	//if (inputPtr->GetKey('D'))
	//	MoveAlongRight(+0.01f);

	//if (inputPtr->GetKey('W'))
	//	MoveAlongForward(+0.01f);
	//if (inputPtr->GetKey('S'))
	//	MoveAlongForward(-0.01f);

	//if (inputPtr->GetKey(' '))
	//	Move(0, +0.01f, 0);
	//if (inputPtr->GetKey('X'))
	//	Move(0, -0.01f, 0);

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

mat4 * Camera::GetReflectionMat()
{
	return &reflectionMatTranposed;
}

vec3 Camera::GetPos()
{
	return position;
}

float Camera::GetHeight()
{
	return height;
}

float Camera::GetWidth()
{
	return width;
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
