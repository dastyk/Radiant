#include "Camera.h"
#include "System.h"

Camera::Camera()
{
	_position = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	_lookAt = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	_up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	_lookDir = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	_right = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	
	_aspect = static_cast<float>(System::GetInstance()->GetWindowHandler()->GetWindowHeight()) / static_cast<float>(System::GetInstance()->GetWindowHandler()->GetWindowWidth());
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

void Camera::SetLookAt(const DirectX::XMFLOAT3& lookAt)
{
	_lookAt = DirectX::XMLoadFloat3(&lookAt);
	_lookAt = DirectX::XMVectorSubtract(_lookAt, _position);
	_lookDir = DirectX::XMVector3Normalize(_lookAt);
	_lookAt = DirectX::XMVectorAdd(_position, _lookDir);
}

void Camera::SetLookDir(const DirectX::XMFLOAT3 & lookDir)
{
	_lookDir = DirectX::XMLoadFloat3(&lookDir);
	_lookAt = DirectX::XMVectorAdd(_position, _lookDir);
	_right = DirectX::XMVector3Cross(_up, _lookDir);
}

void Camera::SetFOV(float fov)
{
	if (fov > 60.0f && fov < 140.0f)
		_fov = fov;
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

void Camera::TranslateAlongVector(const DirectX::XMFLOAT3 & direction, float amount)
{
	DirectX::XMVECTOR v = DirectX::XMLoadFloat3(&direction);
	_position = DirectX::XMVectorAdd(_position, DirectX::XMVectorScale(v, amount));
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
