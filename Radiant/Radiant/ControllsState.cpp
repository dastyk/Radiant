#include "ControllsState.h"
#include "System.h"


ControllsState::ControllsState()
{
}


ControllsState::~ControllsState()
{
}

void ControllsState::Init()
{
	XMFLOAT4 TextColor = XMFLOAT4(41.0f / 255.0f, 127.0f / 255.0f, 185.0f / 255.0f, 1.0f);


	auto i = System::GetInput();
	auto o = System::GetOptions();
	float width = (float)o->GetScreenResolutionWidth();
	float height = (float)o->GetScreenResolutionHeight();
	auto c = _controller;
	//auto a = System::GetInstance()->GetAudio();
	float widthPercentOfDefault = (1.0f / 1920.0f) * width;
	float heightPercentOfDefault = (1.0f / 1080.0f) * height;
	float fontSize = 40 * widthPercentOfDefault;

	float xof = width / 6.0f;
	float OffsetY = 0.0f;
	float OffsetX = width / 2.0f;


	// Background image
	_builder->CreateImage(
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		width,
		height,
		"Assets/Textures/conceptArt.png");

	// Controlls text
	Entity opt = _builder->CreateLabel(
		XMFLOAT3(OffsetX - 100.0f, 25.0f, 0.0f),
		"Controlls",
		fontSize,
		TextColor,
		250.0f,
		45.0f,
		"");

	//Add the "current" height to OffsetY, then set the current text to this offset in Y. This way, we can change positions on everything without having to worry.
	OffsetY += 25.0f*heightPercentOfDefault;
	_builder->Transform()->SetPosition(opt, XMFLOAT3(OffsetX - _builder->Text()->GetLength(opt) / 2.0f, OffsetY, 0.0f));


	// Back button
	Entity b1 = _builder->CreateButton(
		XMFLOAT3(50.0f*widthPercentOfDefault, height - 80.0f*heightPercentOfDefault, 0.0f),
		"Back",
		fontSize,
		TextColor,
		"",
		[this, i]() {
//		a->PlaySoundEffect(L"menuclick.wav", 1);
		ChangeStateTo(StateChange(new MenuState));
	});

	float textSize = 0.0f;
	float xpos = 0.0f;
	
	// Move text
	Entity ftext = _builder->CreateLabel(
		XMFLOAT3(OffsetX - 100.0f, 25.0f, 0.0f),
		"Moving: ",
		fontSize,
		TextColor,
		250.0f,
		45.0f,
		"");

	//Add the "current" height to OffsetY, then set the current text to this offset in Y. This way, we can change positions on everything without having to worry.
	OffsetY += 100.0f*heightPercentOfDefault;
	textSize = _builder->Text()->GetLength(ftext);
	xpos = xof;
	_builder->Transform()->SetPosition(ftext, XMFLOAT3(xpos, OffsetY, 0.0f));

	// Forward button
	Entity fbut = _builder->CreateButton(
		XMFLOAT3(xpos + textSize + 50.0f*widthPercentOfDefault, OffsetY - 50.0f*heightPercentOfDefault, 0.0f),
		"W",
		fontSize,
		TextColor,
		"",
		[this, i]() {
	//	a->PlaySoundEffect(L"menuclick.wav", 1);
	});

	// Back button
	Entity bbut = _builder->CreateButton(
		XMFLOAT3(xpos + textSize + 50.0f*widthPercentOfDefault, OffsetY, 0.0f),
		"S",
		fontSize,
		TextColor,
		"",
		[this, i]() {
		//a->PlaySoundEffect(L"menuclick.wav", 1);
	});

	// Left button
	Entity lbut = _builder->CreateButton(
		XMFLOAT3(xpos + textSize, OffsetY, 0.0f),
		"A",
		fontSize,
		TextColor,
		"",
		[this, i]() {
		//a->PlaySoundEffect(L"menuclick.wav", 1);
	});

	// Right button
	Entity rbut = _builder->CreateButton(
		XMFLOAT3(xpos + textSize + 100.0f*widthPercentOfDefault, OffsetY, 0.0f),
		"D",
		fontSize,
		TextColor,
		"",
		[this, i]() {
		//a->PlaySoundEffect(L"menuclick.wav", 1);
	});



	// jump text
	Entity jumptext = _builder->CreateLabel(
		XMFLOAT3(OffsetX - 100.0f, 25.0f, 0.0f),
		"Jump: ",
		fontSize,
		TextColor,
		250.0f,
		45.0f,
		"");

	//Add the "current" height to OffsetY, then set the current text to this offset in Y. This way, we can change positions on everything without having to worry.
	OffsetY += 100.0f*heightPercentOfDefault;
	textSize = _builder->Text()->GetLength(jumptext);
	xpos = xof;
	_builder->Transform()->SetPosition(jumptext, XMFLOAT3(xpos, OffsetY, 0.0f));

	// jump button
	Entity jumput = _builder->CreateButton(
		XMFLOAT3(xpos + textSize, OffsetY, 0.0f),
		"Space",
		fontSize,
		TextColor,
		"",
		[this, i]() {
		//a->PlaySoundEffect(L"menuclick.wav", 1);
	});




	// dash text
	Entity  dashtext = _builder->CreateLabel(
		XMFLOAT3(OffsetX - 100.0f, 25.0f, 0.0f),
		"Dash: ",
		fontSize,
		TextColor,
		250.0f,
		45.0f,
		"");

	//Add the "current" height to OffsetY, then set the current text to this offset in Y. This way, we can change positions on everything without having to worry.
	OffsetY += 100.0f*heightPercentOfDefault;
	textSize = _builder->Text()->GetLength(dashtext);
	xpos = xof;
	_builder->Transform()->SetPosition(dashtext, XMFLOAT3(xpos, OffsetY, 0.0f));

	// dash button
	Entity dashbut = _builder->CreateButton(
		XMFLOAT3(xpos + textSize, OffsetY, 0.0f),
		"LShift",
		fontSize,
		TextColor,
		"",
		[this, i]() {
		//a->PlaySoundEffect(L"menuclick.wav", 1);
	});













	// Shoot text
	Entity shoottext = _builder->CreateLabel(
		XMFLOAT3(OffsetX - 100.0f, 25.0f, 0.0f),
		"Shoot: ",
		fontSize,
		TextColor,
		250.0f,
		45.0f,
		"");

	//Add the "current" height to OffsetY, then set the current text to this offset in Y. This way, we can change positions on everything without having to worry.
	OffsetY += 100.0f*heightPercentOfDefault;
	textSize = _builder->Text()->GetLength(shoottext);
	xpos = xof;
	_builder->Transform()->SetPosition(shoottext, XMFLOAT3(xpos, OffsetY, 0.0f));

	// shoot button
	Entity shootbut = _builder->CreateButton(
		XMFLOAT3(xpos + textSize, OffsetY, 0.0f),
		"LMouse",
		fontSize,
		TextColor,
		"",
		[this, i]() {
		//a->PlaySoundEffect(L"menuclick.wav", 1);
	});

	// Activatepower text
	Entity  powertext = _builder->CreateLabel(
		XMFLOAT3(OffsetX - 100.0f, 25.0f, 0.0f),
		"Activate Power: ",
		fontSize,
		TextColor,
		250.0f,
		45.0f,
		"");

	//Add the "current" height to OffsetY, then set the current text to this offset in Y. This way, we can change positions on everything without having to worry.
	OffsetY += 100.0f*heightPercentOfDefault;
	textSize = _builder->Text()->GetLength(powertext);
	xpos = xof;
	_builder->Transform()->SetPosition(powertext, XMFLOAT3(xpos, OffsetY, 0.0f));

	// Activatepower button
	Entity powerbut = _builder->CreateButton(
		XMFLOAT3(xpos + textSize, OffsetY, 0.0f),
		"RMouse",
		fontSize,
		TextColor,
		"",
		[this, i]() {
		//a->PlaySoundEffect(L"menuclick.wav", 1);
	});

	// next power text
	Entity  nextptext = _builder->CreateLabel(
		XMFLOAT3(OffsetX - 100.0f, 25.0f, 0.0f),
		"Next Power: ",
		fontSize,
		TextColor,
		250.0f,
		45.0f,
		"");

	//Add the "current" height to OffsetY, then set the current text to this offset in Y. This way, we can change positions on everything without having to worry.
	OffsetY += 100.0f*heightPercentOfDefault;
	textSize = _builder->Text()->GetLength(nextptext);
	xpos = xof;
	_builder->Transform()->SetPosition(nextptext, XMFLOAT3(xpos, OffsetY, 0.0f));

	// next power button
	Entity nextpbut = _builder->CreateButton(
		XMFLOAT3(xpos + textSize, OffsetY, 0.0f),
		"Q",
		fontSize,
		TextColor,
		"",
		[this, i]() {
		//a->PlaySoundEffect(L"menuclick.wav", 1);
	});





	// Cycle wep up text
	Entity  cyuptext = _builder->CreateLabel(
		XMFLOAT3(OffsetX - 100.0f, 25.0f, 0.0f),
		"Next Weapon: ",
		fontSize,
		TextColor,
		250.0f,
		45.0f,
		"");

	//Add the "current" height to OffsetY, then set the current text to this offset in Y. This way, we can change positions on everything without having to worry.
	OffsetY += 100.0f*heightPercentOfDefault;
	textSize = _builder->Text()->GetLength(cyuptext);
	xpos = xof;
	_builder->Transform()->SetPosition(cyuptext, XMFLOAT3(xpos, OffsetY, 0.0f));

	// Cycle wep up button
	Entity cyupbut = _builder->CreateButton(
		XMFLOAT3(xpos + textSize, OffsetY, 0.0f),
		"Scroll Up",
		fontSize,
		TextColor,
		"",
		[this, i]() {
		//a->PlaySoundEffect(L"menuclick.wav", 1);
	});

	// Cycle wep down text
	Entity  cydowntext = _builder->CreateLabel(
		XMFLOAT3(OffsetX - 100.0f, 25.0f, 0.0f),
		"Previous Weapon: ",
		fontSize,
		TextColor,
		250.0f,
		45.0f,
		"");

	//Add the "current" height to OffsetY, then set the current text to this offset in Y. This way, we can change positions on everything without having to worry.
	OffsetY += 100.0f*heightPercentOfDefault;
	textSize = _builder->Text()->GetLength(cydowntext);
	xpos = xof;
	_builder->Transform()->SetPosition(cydowntext, XMFLOAT3(xpos, OffsetY, 0.0f));

	// Cycle wep down button
	Entity cydownbut = _builder->CreateButton(
		XMFLOAT3(xpos + textSize, OffsetY, 0.0f),
		"Scroll Down",
		fontSize,
		TextColor,
		"",
		[this, i]() {
		//a->PlaySoundEffect(L"menuclick.wav", 1);
	});


}

void ControllsState::Shutdown()
{
}
