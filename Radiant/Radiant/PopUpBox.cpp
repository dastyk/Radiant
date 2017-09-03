#include "PopUpBox.h"
#include "System.h"


PopUpBox::PopUpBox(PopUpType& type, const std::string& text, std::function<void(unsigned int)> callback) :EntityBuilder(), _type(type), _width(250), _height(100), _callback(std::move(callback))
{



	auto o = System::GetOptions();
	_pos = XMFLOAT3(o->GetScreenResolutionWidth()/ 2.0f - _width, o->GetScreenResolutionHeight() / 2.0f - _height, 0.0f);

	switch (type)
	{
	case PopUpType::YESNO:
		CreateLabel(XMFLOAT3(_pos.x + _width / 2.0f - _width / 4.0f, 5.0f, 0.0f),
			text,
			40,
			_color,
			_width / 4.0f,
			50.0f,
			"");

		CreateButton(
			XMFLOAT3(_pos.x + 5.0f, _pos.y + 25, 0.0f),
			"Yes",
			40,
			_color,
			50,
			50,
			"",
			[this]() {this->_callback(0); });

		CreateButton(
			XMFLOAT3(_pos.x + _width - 55.0f, _pos.y + 25, 0.0f),
			"No",
			40,
			_color,
			50,
			50,
			"",
			[this]() {this->_callback(1); });
		CreateImage(_pos, _width, _height, "Assets/Textures/ft_stone01_c.png");
		break;
	case PopUpType::OK:
		break;
	default:
		break;
	}
}
PopUpBox::~PopUpBox()
{

}
const void PopUpBox::Update()const
{
	_controller->Update();
}
const void PopUpBox::Render()const
{
	_controller->SetExclusiveRenderAccess();
	System::GetGraphics()->Render(0.0, 0.0);
}