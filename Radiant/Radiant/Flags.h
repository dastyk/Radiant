#ifndef _FLAGS_H_
#define _FLAGS_H_

#pragma once
template<class ENUM, class UNDERLYING = typename std::underlying_type<ENUM>::type>
class SafeEnum
{
public:
	SafeEnum() : mFlags(0) {}
	SafeEnum(ENUM singleFlag) : mFlags(singleFlag) {}
	SafeEnum(const SafeEnum& original) : mFlags(original.mFlags) {}

	SafeEnum&   operator |=(ENUM addValue) { mFlags |= addValue; return *this; }
	SafeEnum    operator |(ENUM addValue) { SafeEnum  result(*this); result |= addValue; return result; }
	SafeEnum&   operator &=(ENUM maskValue) { mFlags &= maskValue; return *this; }
	SafeEnum    operator &(ENUM maskValue) { SafeEnum  result(*this); result &= maskValue; return result; }
	SafeEnum    operator ~() { SafeEnum  result(*this); result.mFlags = ~result.mFlags; return result; }
	unsigned int Count() { unsigned int count = 0; for (unsigned int i = 0; i < sizeof(UNDERLYING); i++) { count += mFlags << i; } }
	operator bool() { return mFlags != 0; }

protected:
	UNDERLYING  mFlags;
};

#endif