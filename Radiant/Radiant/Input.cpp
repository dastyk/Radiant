#include "Input.h"



Input::Input()
{
}


Input::~Input()
{
}

const void Input::Init()
{
	for (uint i = 0; i < NROFKEYS; i++)
	{
		_keys[i] = false;
	}


	return void();
}

const void Input::ShutDown()
{

	return void();
}

const void Input::KeyDown(uint keyCode)
{
	if (keyCode >= NROFKEYS || keyCode < 0)
		throw ErrorMsg(4000001, L"Key out of bounds" + std::to_wstring(keyCode));
	_keys[keyCode] = true;
	return void();
}

const void Input::KeyUp(uint keyCode)
{
	if (keyCode >= NROFKEYS || keyCode < 0)
		throw ErrorMsg(4000001, L"Key out of bounds" + std::to_wstring(keyCode));
	_keys[keyCode] = false;
	return void();
}

const bool Input::IsKeyDown(uint keyCode) const
{
	if (keyCode >= NROFKEYS || keyCode < 0)
		throw ErrorMsg(4000001, L"Key out of bounds" + std::to_wstring(keyCode));
	return _keys[keyCode];
}

const bool Input::GetKeyStateAndReset(uint keyCode)
{
	if (keyCode >= NROFKEYS || keyCode < 0)
		throw ErrorMsg(4000001, L"Key out of bounds" + std::to_wstring(keyCode));
	bool out = _keys[keyCode];
	_keys[keyCode] = false;
	return out;
}

const void Input::OnMouseMove(int x, int y)
{
	_lastMousePosX = _mousePosX;
	_lastMousePosY = _mousePosY;
	_mousePosX = x;
	_mousePosY = y;
	return void();
}

const void Input::MouseDown(uint keyCode)
{
	if (keyCode >= NROFMOUSEKEYS || keyCode < 0)
		throw ErrorMsg(4000002, L"Mouse Key out of bounds" + std::to_wstring(keyCode));
	_mouseKeys[keyCode] = true;
	return void();
}

const void Input::MouseUp(uint keyCode)
{
	if (keyCode >= NROFMOUSEKEYS || keyCode < 0)
		throw ErrorMsg(4000002, L"Mouse Key out of bounds" + std::to_wstring(keyCode));
	_mouseKeys[keyCode] = true;
	return void();
}

const bool Input::IsMouseKeyDown(uint keyCode) const
{
	if (keyCode >= NROFMOUSEKEYS || keyCode < 0)
		throw ErrorMsg(4000002, L"Mouse Key out of bounds" + std::to_wstring(keyCode));
	return _mouseKeys[keyCode];
}

const bool Input::GetMouseKeyStateAndReset(uint keyCode)
{
	if (keyCode >= NROFMOUSEKEYS || keyCode < 0)
		throw ErrorMsg(4000002, L"Mouse Key out of bounds" + std::to_wstring(keyCode));
	bool out = _mouseKeys[keyCode];
	_mouseKeys[keyCode] = false;
	return out;
}

const void Input::GetMousePos(int & x, int & y) const
{
	x = _mousePosX;
	y = _mousePosY;
	return void();
}
