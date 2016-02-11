#include "TextManager.h"
#include "Utils.h"
#include "System.h"

TextManager::TextManager(TransformManager& trans)
{
	System::GetGraphics()->AddTextProvider(this);
	trans.SetTransformChangeCallback7([this](const Entity& entity, const DirectX::XMVECTOR& pos)
	{
		_TransformChanged(entity, pos);
	});
}


TextManager::~TextManager()
{
	for (auto& f : _loadedFonts)
	{
		delete[]f.second->Font;
		delete f.second;
	}
	_loadedFonts.clear();
}

void TextManager::GatherTextJobs(TextJob2& jobs)
{
	for (auto& t : _textStrings)
	{
		if(t.visible)
			jobs[t.font->texture][t.VertexBuffer] = &t;
	}
}

const void TextManager::BindText(const Entity & entity, const std::string&  text, const std::string& fontName, uint fontSize, const XMFLOAT4& Color)
{
	auto indexIt = _entityToIndex.find(entity);

	if (indexIt != _entityToIndex.end())
	{
		TraceDebug("Tried to bind text component to entity that already had one.");
		return;
	}

	auto g = System::GetGraphics();

	FontData data;

	data.font = LoadFont(fontName);
	data.text = text;
	data.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	data.FontSize = (float)fontSize / (float)data.font->refSize;
	data.Color = std::move(Color);
	data.visible = true;
	try { data.VertexBuffer = g->CreateTextBuffer(data); }
	catch (ErrorMsg& msg)
	{
		msg;
		TraceDebug("Could not create text buffers.");
		return;
	}

	_textStrings.push_back(move(data));
	_entityToIndex[entity] = static_cast<unsigned int>(_textStrings.size() - 1);;
	return void();
}

const void TextManager::ChangeText(const Entity & entity, const std::string& text)
{
	auto indexIt = _entityToIndex.find(entity);

	if (indexIt != _entityToIndex.end())
	{
		_textStrings[indexIt->second].text = text;
		System::GetGraphics()->UpdateTextBuffer(_textStrings[indexIt->second].VertexBuffer, _textStrings[indexIt->second]);
		return;
	}
	TraceDebug("Tried to change text for an entity that had no text component.");
}

const void TextManager::ChangeFontSize(const Entity & entity, uint fontSize)
{
	auto indexIt = _entityToIndex.find(entity);

	if (indexIt != _entityToIndex.end())
	{
		_textStrings[indexIt->second].FontSize = (float)fontSize / (float)_textStrings[indexIt->second].font->refSize;
		System::GetGraphics()->UpdateTextBuffer(_textStrings[indexIt->second].VertexBuffer, _textStrings[indexIt->second]);
		return;
	}
	TraceDebug("Tried to change fontsize for an entity that had no text component.");
}

const void TextManager::ChangeColor(const Entity & entity, const XMFLOAT4 & Color)
{
	auto indexIt = _entityToIndex.find(entity);

	if (indexIt != _entityToIndex.end())
	{
		_textStrings[indexIt->second].Color = std::move(Color);
		return;
	}
	TraceDebug("Tried to change color for an entity that had no text component.");
}

const void TextManager::ToggleVisible(const Entity & entity, bool visible)
{
	auto indexIt = _entityToIndex.find(entity);

	if (indexIt != _entityToIndex.end())
	{

		_textStrings[indexIt->second].visible = visible;
		return;
	}
	TraceDebug("Tried to change visability for an entity that had no text component.");
}

const void TextManager::BindToRenderer(bool exclusive)
{
	if (exclusive)
		System::GetGraphics()->ClearTextProviders();

	System::GetGraphics()->AddTextProvider(this);
	return void();
}

const std::string& TextManager::GetText(const Entity & entity) const
{
	auto index = _entityToIndex.find(entity);
	if (index != _entityToIndex.end())
	{
		return _textStrings[index->second].text;
	}
	TraceDebug("Tried to get text from an entity with no text manager.");
	return std::string("");
}

const void TextManager::_TransformChanged(const Entity & entity, const DirectX::XMVECTOR & pos)
{
	auto indexIt = _entityToIndex.find(entity);

	if (indexIt != _entityToIndex.end())
	{
		DirectX::XMStoreFloat3(&_textStrings[indexIt->second].pos, pos);
		System::GetGraphics()->UpdateTextBuffer(_textStrings[indexIt->second].VertexBuffer, _textStrings[indexIt->second]);
		return;
	}
	return void();
}

Fonts * TextManager::LoadFont(const std::string& fontName)
{
	LPCSTR st = fontName.c_str();
	string fn = PathFindFileNameA(st);
	auto indexIt = _loadedFonts.find(fn);

	if (indexIt != _loadedFonts.end())
	{
		TraceDebug("Font already loaded, passed the loaded one.");
		return indexIt->second;
	}



	ifstream fin;
	int i;
	char temp;
	Fonts * font = new Fonts;

	try { font->texture = System::GetGraphics()->CreateTexture(S2WS(fontName + ".dds").c_str()); }
	catch (ErrorMsg& msg)
	{
		delete font;
		throw msg;
	}

	// Read in the font size and spacing between chars.
	fin.open(fontName + ".txt");
	if (fin.fail())
	{
		return nullptr;
	}	
	
	fin >> font->nroffonts;
	font->Font = new FontType[font->nroffonts];
	if (!font->Font)
	{
		return nullptr;
	}

	fin >> font->offset;
	fin >> font->refSize;
	fin >> font->tsize;

	// Read in the ascii characters for text.
	for (i = 0; i < font->nroffonts; i++)
	{
		fin.get(temp);
		while (temp != ' ')
		{
			fin.get(temp);
		}
		fin.get(temp);
		while (temp != ' ')
		{
			fin.get(temp);
		}

		fin >> font->Font[i].left;
		font->Font[i].left /= (float)font->tsize;
		fin >> font->Font[i].size;
		font->Font[i].right = font->Font[i].left + font->Font[i].size/(float)font->tsize;

	}

	// Close the file.
	fin.close();

	return _loadedFonts[fn] = font;
}
