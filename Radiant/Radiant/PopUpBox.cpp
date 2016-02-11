#include "PopUpBox.h"
#include "System.h"


PopUpBox::PopUpBox(Type& type, const std::string& text) :EntityBuilder(), _type(type), _width(250), _height(100)
{
	auto h = System::GetWindowHandler();
	_pos = XMFLOAT3(h->GetScreenWidth() / 2.0f - _width, h->GetScreenHeight() / 2.0f - _height, 0.0f);

	switch (type)
	{
	case Type::YESNO:
		CreateLabel(XMFLOAT3(_pos.x + _width / 2.0f - _width / 4.0f, 5.0f, 0.0f),
			text,
			_color,
			_width / 4.0f,
			50.0f,
			"");

		CreateButton(
			XMFLOAT3(_pos.x + 5.0f, _pos.y + 25, 0.0f),
			"Yes",
			_color,
			50,
			50,
			"",
			[this]() {this->_valueset = true; this->_value = 0; });

		CreateButton(
			XMFLOAT3(_pos.x + _width - 55.0f, _pos.y + 25, 0.0f),
			"No",
			_color,
			50,
			50,
			"",
			[this]() {this->_valueset = true; this->_value = 1; });
		CreateImage(_pos, _width, _height, "Assets/Textures/ft_stone01_c.png");
		break;
	case Type::OK:
		break;
	default:
		break;
	}
}
PopUpBox::~PopUpBox()
{

}
const uint PopUpBox::GetValue()const
{
	return _value;
}
const bool PopUpBox::Update()const
{
	_controller->Update();
	return _valueset;
}
const void PopUpBox::Render()const
{
	_controller->SetExclusiveRenderAccess();
	System::GetGraphics()->Render(0.0, 0.0);
}