#ifndef _POPUPBOX_H_
#define _POPUPBOX_H_
#pragma once
#include "EntityBuilder.h"

class PopUpBox :
	public EntityBuilder
{
public:
	PopUpBox(PopUpType& type, const std::string& text, std::function<void(unsigned int)> callback);
	~PopUpBox();
	const void Update()const;
	const void Render()const;

private:
	PopUpType _type;
	XMFLOAT3 _pos;
	float _width;
	float _height;
	XMFLOAT4 _color = XMFLOAT4(0.1f, 0.3f, 0.6f, 1.0f);
	std::function<void(unsigned int)> _callback;
};

#endif