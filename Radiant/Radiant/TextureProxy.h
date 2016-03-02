#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <cstdint>

class TextureProxy
{
	friend class Graphics;

public:
	TextureProxy() : Type(Type::Regular), Index(1) {}

private:
	enum class Type
	{
		Regular,
		Structured,
		StructuredDynamic
	};

private:
	TextureProxy(Type type, std::uint32_t index) : Type(type), Index(index) {}

public: // Meh, want private
	Type Type;
	std::uint32_t Index;
};

#endif