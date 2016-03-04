#include "StaticObject.h"

StaticObject::StaticObject():_position(DirectX::XMVectorSet(0.0f,0.0f,0.0f,0.0f)),_scale(DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f)),_rotate(DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f))
{

}

StaticObject::StaticObject(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& scale, const DirectX::XMFLOAT3& rotAxis, float radians)
{
	SetPosition(pos);
	SetScale(scale);
	SetRotation(rotAxis, radians);
}

StaticObject::~StaticObject()
{

}

void StaticObject::SetPosition(const DirectX::XMFLOAT3& pos)
{
	_position = DirectX::XMLoadFloat3(&pos);
	DirectX::XMVectorSetW(_position, 1.0f);
}

void StaticObject::SetScale(const DirectX::XMFLOAT3& scale)
{
	_scale = DirectX::XMLoadFloat3(&scale);
}

void StaticObject::SetRotation(const DirectX::XMFLOAT3& axis, float radians)
{
	_rotate = DirectX::XMVectorSet(axis.x, axis.y, axis.z, radians);
}

DirectX::XMFLOAT4X4 StaticObject::GetTranslationMatrix()
{
	DirectX::XMMATRIX m = DirectX::XMMatrixTranslationFromVector(_position);
	DirectX::XMFLOAT4X4 mf;
	DirectX::XMStoreFloat4x4(&mf, m);
	return mf;
}

DirectX::XMFLOAT4X4 StaticObject::GetScalingMatrix()
{
	DirectX::XMMATRIX m = DirectX::XMMatrixScalingFromVector(_scale);
	DirectX::XMFLOAT4X4 mf;
	DirectX::XMStoreFloat4x4(&mf, m);
	return mf;
}

DirectX::XMFLOAT4X4 StaticObject::GetRotationMatrix()
{
	DirectX::XMMATRIX m = DirectX::XMMatrixRotationAxis(_rotate, DirectX::XMVectorGetW(_rotate));
	DirectX::XMFLOAT4X4 mf;
	DirectX::XMStoreFloat4x4(&mf, m);
	return mf;
}

DirectX::XMFLOAT4X4 StaticObject::GetWorldMatrix()
{
	DirectX::XMMATRIX t = DirectX::XMMatrixTranslationFromVector(_position);
	DirectX::XMMATRIX s = DirectX::XMMatrixScalingFromVector(_scale);
	DirectX::XMMATRIX r = DirectX::XMMatrixRotationAxis(_rotate, DirectX::XMVectorGetW(_rotate));

	DirectX::XMFLOAT4X4 srt;
	DirectX::XMStoreFloat4x4(&srt, s * r * t);
	return srt;
}