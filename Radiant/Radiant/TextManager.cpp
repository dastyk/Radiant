#include "TextManager.h"
#include "Utils.h"
#include "System.h"

using namespace DirectX;

TextManager::TextManager(TransformManager& trans)
{
	System::GetGraphics()->AddTextProvider(this);

	trans.TransformChanged += Delegate<void( const Entity&, const XMMATRIX&, const XMVECTOR&, const XMVECTOR&, const XMVECTOR& )>::Make<TextManager, &TextManager::_TransformChanged>( this );
}


TextManager::~TextManager()
{
	auto g = System::GetGraphics();
	std::vector<Entity> v;
	for (auto& o : _entityToData)
		v.push_back(std::move(o.first));

	for (auto& o : v)
		ReleaseText(o);

	for (auto& f : _loadedFonts)
	{
		System::GetGraphics()->ReleaseTexture(f.second->texture);
		SAFE_DELETE_ARRAY(f.second->Font);
		SAFE_DELETE(f.second);
	}
	_loadedFonts.clear();
}

void TextManager::GatherTextJobs(TextJob& jobs)
{
	for (auto& t : _entityToData)
	{
		if(t.second->visible)
			jobs[t.second->font->texture].push_back( t.second);
	}
}

const void TextManager::BindText(const Entity & entity, const std::string&  text, const std::string& fontName, uint fontSize, const XMFLOAT4& Color)
{
	auto indexIt = _entityToData.find(entity);

	if (indexIt != _entityToData.end())
	{
		return;
	}

	auto g = System::GetGraphics();

	FontData* data = nullptr;
	try { data = new FontData; }
	catch (std::exception& e) { e; SAFE_DELETE(data); throw ErrorMsg(1700001,L"Failed to create fontdata."); }
	data->font = LoadFont(fontName);
	data->text = text;
	data->pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	data->FontSize = (float)fontSize / (float)data->font->refSize;
	data->Color = std::move(Color);
	data->visible = true;
	try { data->VertexBuffer = g->CreateTextBuffer(data); }
	catch (ErrorMsg& msg)
	{
		msg;
		SAFE_DELETE(data);
		TraceDebug("Could not create text buffers.");
		return;
	}
	_entityToData[entity] = data;

	return void();
}

const void TextManager::ChangeText(const Entity & entity, const std::string& text)
{
	auto indexIt = _entityToData.find(entity);

	if (indexIt != _entityToData.end())
	{
		indexIt->second->text = text;
		System::GetGraphics()->UpdateTextBuffer(indexIt->second);
		return;
	}
}

const void TextManager::ChangeFontSize(const Entity & entity, uint fontSize)
{
	auto indexIt = _entityToData.find(entity);

	if (indexIt != _entityToData.end())
	{
		indexIt->second->FontSize = (float)fontSize / (float)indexIt->second->font->refSize;
		System::GetGraphics()->UpdateTextBuffer(indexIt->second);
		return;
	}
}

const void TextManager::ChangeColor(const Entity & entity, const XMFLOAT4 & Color)
{
	auto indexIt = _entityToData.find(entity);

	if (indexIt != _entityToData.end())
	{
		indexIt->second->Color = std::move(Color);
		return;
	}
}

const void TextManager::ReleaseText(const Entity & entity)
{
	auto got = _entityToData.find(entity);

	if (got == _entityToData.end())
	{
		return;
	}

	System::GetGraphics()->ReleaseDynamicVertexBuffer(got->second->VertexBuffer);

	SAFE_DELETE(got->second);
	_entityToData.erase(entity);
	return void();
}

const void TextManager::ToggleVisible(const Entity & entity, bool visible)
{
	auto indexIt = _entityToData.find(entity);

	if (indexIt != _entityToData.end())
	{

		indexIt->second->visible = visible;
		return;
	}
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
	auto index = _entityToData.find(entity);
	if (index != _entityToData.end())
	{
		return index->second->text;
	}
	return std::string("");
}

void TextManager::_TransformChanged( const Entity& entity, const XMMATRIX& tran, const XMVECTOR& pos, const XMVECTOR& dir, const XMVECTOR& up )
{
	auto indexIt = _entityToData.find(entity);

	if (indexIt != _entityToData.end())
	{
		DirectX::XMStoreFloat3(&indexIt->second->pos, pos);
		System::GetGraphics()->UpdateTextBuffer(indexIt->second);
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
		return indexIt->second;
	}



	ifstream fin;
	uint i;
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
	fin >> font->offset;
	font->Font = new FontType[font->offset + font->nroffonts];
	if (!font->Font)
	{
		return nullptr;
	}


	fin >> font->refSize;
	fin >> font->tsize;

	// Read in the ascii characters for text.
	for (i = font->offset; i < font->offset + font->nroffonts; i++)
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
