#ifndef _CAMERA_H_
#define _CAMERA_H_
#pragma once
#include <DirectXMath.h>

class Camera
{
public:
	Camera();
	~Camera();

	/* Must be called to update the matrices */
	void Update();
	
	void MoveForward(float amount);
	void MoveBackward(float amount);
	void MoveRight(float amount);
	void MoveLeft(float amount);
	void RotateYaw(float radians);
	void RotatePitch(float radians);

	void SetPosition(const DirectX::XMFLOAT3& position);
	void SetLookAt(const DirectX::XMFLOAT3& lookAt);
	void SetLookDir(const DirectX::XMFLOAT3& lookDir);
	void SetFOV(float fov);
	void SetNearDistance(float nearDistance);
	void SetFarDistance(float farDistance);
	void SetAspectRatio(float aspect);

	void TranslateAlongVector(const DirectX::XMFLOAT3& direction, float amount);

	DirectX::XMFLOAT4X4 GetViewMatrix();
	DirectX::XMFLOAT4X4 GetProjectionMatrix();
	DirectX::XMFLOAT4X4 GetViewProjectionMatrix();

private:
	
	DirectX::XMVECTOR _position;
	DirectX::XMVECTOR _lookAt;
	DirectX::XMVECTOR _up;
	DirectX::XMVECTOR _lookDir;
	DirectX::XMVECTOR _right;

	float _fov;
	float _aspect;
	float _near;
	float _far;

	DirectX::XMFLOAT4X4 _viewMatrix;
	DirectX::XMFLOAT4X4 _projectionMatrix;
	DirectX::XMFLOAT4X4 _viewProjectionMatrix;
};


#endif

