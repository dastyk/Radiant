#include "Camera.h"
#include "System.h"

Camera::Camera()
{
	_position = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	_lookAt = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	_up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	_lookDir = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	_right = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	
	auto o = System::GetOptions();
	_aspect = static_cast<float>(o->GetScreenResolutionWidth()) / static_cast<float>(o->GetScreenResolutionHeight());
	_fov = 90.0f;
	_near = 1.0f;
	_far = 100.0f;

	Update();
}

Camera::~Camera()
{
}

void Camera::Update()
{
	DirectX::XMStoreFloat4x4(&_viewMatrix, DirectX::XMMatrixLookAtLH(_position, _lookAt, _up));
	DirectX::XMStoreFloat4x4(&_projectionMatrix, DirectX::XMMatrixPerspectiveFovLH(_fov, _aspect, _near, _far));
	DirectX::XMStoreFloat4x4(&_viewProjectionMatrix, DirectX::XMLoadFloat4x4(&_viewMatrix) * DirectX::XMLoadFloat4x4(&_projectionMatrix));
}

void Camera::MoveForward(float amount)
{
	_position = DirectX::XMVectorAdd(_position, DirectX::XMVectorScale(_lookDir, amount));
}

void Camera::MoveBackward(float amount)
{
	MoveForward(-amount);
}

void Camera::MoveRight(float amount)
{
	_position = DirectX::XMVectorAdd(_position, DirectX::XMVectorScale(_right, amount));
}

void Camera::MoveLeft(float amount)
{
	MoveRight(-amount);
}

/* Look left/right basically */
void Camera::RotateYaw(float radians)
{
	DirectX::XMMATRIX rotate = DirectX::XMMatrixRotationY(radians);
	_lookDir = DirectX::XMVector3Transform(_lookDir, rotate);
	_lookAt = DirectX::XMVectorAdd(_position, _lookDir);
	_right = DirectX::XMVector3Cross(_up, _lookDir);
}

/* Look up/down basically */
void Camera::RotatePitch(float radians)
{
	DirectX::XMMATRIX rotate = DirectX::XMMatrixRotationX(radians);
	_lookDir = DirectX::XMVector3Transform(_lookDir, rotate);
	_lookAt = DirectX::XMVectorAdd(_position, _lookDir);
	_up = DirectX::XMVector3Cross(_right, _lookDir);
}

void Camera::SetPosition(const DirectX::XMFLOAT3& position)
{
	_position = DirectX::XMLoadFloat3(&position);
}

void Camera::SetPosition(const DirectX::XMVECTOR & position)
{
	_position = position;
}

void Camera::SetLookAt(const DirectX::XMFLOAT3& lookAt)
{
	_lookAt = DirectX::XMLoadFloat3(&lookAt);
	_lookAt = DirectX::XMVectorSubtract(_lookAt, _position);
	_lookDir = DirectX::XMVector3Normalize(_lookAt);
	_lookAt = DirectX::XMVectorAdd(_position, _lookDir);
}

void Camera::SetLookAt(const DirectX::XMVECTOR & lookAt)
{
	_lookAt = lookAt;
}

void Camera::SetLookDir(const DirectX::XMFLOAT3 & lookDir)
{
	_lookDir = DirectX::XMLoadFloat3(&lookDir);
	_lookAt = DirectX::XMVectorAdd(_position, _lookDir);
	_right = DirectX::XMVector3Cross(_up, _lookDir);
}

void Camera::SetLookDir(const DirectX::XMVECTOR & lookAt)
{
	_lookAt = lookAt;
}

void Camera::SetFOV(float fovRadians)
{
	if (fovRadians > 30.0f && fovRadians < 140.0f)
		_fov = fovRadians;
}

void Camera::SetAspectRatio(float aspect)
{
	if(aspect > 0)
		_aspect = aspect;
}

void Camera::SetNearDistance(float nearDistance)
{
	if(nearDistance < _far && nearDistance > 0.0f)
		_near = nearDistance;
}

void Camera::SetFarDistance(float farDistance)
{
	if(farDistance > _near)
		_far = farDistance;
}



DirectX::XMFLOAT4X4 Camera::GetViewMatrix()
{
	return _viewMatrix;
}


DirectX::XMFLOAT4X4 Camera::GetProjectionMatrix()
{
	return _projectionMatrix;
}

DirectX::XMFLOAT4X4 Camera::GetViewProjectionMatrix()
{
	return _viewProjectionMatrix;
}

DirectX::XMFLOAT3 Camera::GetPosition()
{
	DirectX::XMFLOAT3 p;
	DirectX::XMStoreFloat3(&p, _position);
	return p;
}

DirectX::XMFLOAT3 Camera::GetUp()
{
	DirectX::XMFLOAT3 u;
	DirectX::XMStoreFloat3(&u, _up);
	return u;
}

DirectX::XMFLOAT3 Camera::GetLookDir()
{
	DirectX::XMFLOAT3 l;
	DirectX::XMStoreFloat3(&l, _lookDir);
	return l;
}