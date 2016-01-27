#ifndef _CAMERA_H_
#define _CAMERA_H_
#pragma once
#include <DirectXMath.h>
#include "MovableObject.h"

class Camera : public MovableObject
{
public:
	Camera();
	virtual ~Camera();

	/* Must be called to update the matrices */
	virtual void Update();
	
	void MoveForward(float amount);
	void MoveBackward(float amount);
	void MoveRight(float amount);
	void MoveLeft(float amount);
	void RotateYaw(float radians);
	void RotatePitch(float radians);

	void SetPosition(const DirectX::XMFLOAT3& position);
	void SetPosition(const DirectX::XMVECTOR& position);
	void SetLookAt(const DirectX::XMFLOAT3& lookAt);
	void SetLookAt(const DirectX::XMVECTOR& lookAt);
	void SetLookDir(const DirectX::XMFLOAT3& lookDir);
	void SetLookDir(const DirectX::XMVECTOR& lookAt);
	void SetFOV(float fovRadians);
	void SetNearDistance(float nearDistance);
	void SetFarDistance(float farDistance);
	void SetAspectRatio(float aspect);

	

	DirectX::XMFLOAT4X4 GetViewMatrix();
	DirectX::XMFLOAT4X4 GetProjectionMatrix();
	DirectX::XMFLOAT4X4 GetViewProjectionMatrix();

	DirectX::XMFLOAT3 GetPosition();
	DirectX::XMFLOAT3 GetUp();
	DirectX::XMFLOAT3 GetLookDir();

protected:
	
	
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

