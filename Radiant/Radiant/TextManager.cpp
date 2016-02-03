#include "TextManager.h"
#include "Utils.h"
#include "System.h"

TextManager::TextManager(TransformManager& trans)
{
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

void TextManager::GatherTextJobs(std::function<void(FontData&)> ProvideJob)
{
}

const void TextManager::BindText(const Entity & entity, std::string  text, std::string fontName, uint fontSize)
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
	data.FontSize = fontSize;
	try { data.VertexBuffer = g->CreateTextBuffer(data); }
	catch (ErrorMsg& msg)
	{
		msg;
		TraceDebug("Could not create text buffers.");
		return;
	}

	_textStrings.push_back(move(data));
	return void();
}

const void TextManager::ChangeText(const Entity & entity, std::string  text, std::string fontName)
{
	auto indexIt = _entityToIndex.find(entity);

	if (indexIt != _entityToIndex.end())
	{
		
		return;
	}
	TraceDebug("Tried to change text for an entity that had no text component.");
}

const void TextManager::ChangeFontSize(const Entity & entity, uint fontSize)
{
	auto indexIt = _entityToIndex.find(entity);

	if (indexIt != _entityToIndex.end())
	{

		return;
	}
	TraceDebug("Tried to change fontsize for an entity that had no text component.");
}

Fonts * TextManager::LoadFont(std::string & fontName)
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
		fin >> font->Font[i].right;
		fin >> font->Font[i].size;
	}

	// Close the file.
	fin.close();

	return _loadedFonts[fn] = font;
}
