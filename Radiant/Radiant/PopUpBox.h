#ifndef _POPUPBOX_H_
#define _POPUPBOX_H_
#pragma once
#include "EntityBuilder.h"

class PopUpBox :
	public EntityBuilder
{
public:
	enum class Type
	{
		YESNO = 1 << 1,
		OK = 1 << 2
	};

public:
	PopUpBox(Type& type, const std::string& text);
	~PopUpBox();
	const uint GetValue()const;
	const bool Update()const;
	const void Render()const;

private:
	Type _type;
	XMFLOAT3 _pos;
	float _width;
	float _height;
	bool _valueset = false;
	uint _value;
	XMFLOAT4 _color = XMFLOAT4(0.1f, 0.3f, 0.6f, 1.0f);
};

#endif