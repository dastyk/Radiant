#ifndef _FLAGS_H_
#define _FLAGS_H_

#pragma once
#include <vector>

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

#define CCR(name,type, count,...) \
class name\
{\
public:\
static const type __VA_ARGS__;\
static const unsigned int count;\
name() : _flags(type()) {}\
name(type singleFlag) : _flags(singleFlag) {}\
name(const name& original) : _flags(original._flags) {}\
bool operator==(const name& rhs) const { return _flags == rhs._flags; }\
name    operator =(type addValue) { this->_flags = addValue; return *this; }\
name&   operator |=(type addValue) { _flags |= addValue; return *this; }\
name    operator |(type addValue) { name  result(*this); result |= addValue; return result; }\
name&   operator &=(type maskValue) { _flags &= maskValue; return *this; }\
name    operator &(type maskValue) { name  result(*this); result &= maskValue; return result; }\
name    operator ~() { name  result(*this); result._flags = ~result._flags; return result; }\
unsigned int Count() { unsigned int cc = 0; for (unsigned int i = 0; i < sizeof(type); i++) { cc += _flags << i; } return cc; }\
operator type() { return _flags; }\
type _flags;\
std::size_t operator()(const name& key) const{return std::hash<type>()(key._flags);}\
}

#define CreateFlags(type, name, values, maxv) { \
class name \
{ \
public: \
	values ;\
	maxv; \
\
	name() : _flags(0) {}\
	name(type singleFlag) : _flags(singleFlag) {}\
	name(const name& original) : _flags(original._flags) {}\
\
	bool operator==(const name& rhs) const\
	{\
		return _flags == rhs._flags;\
	}\
	name    operator =(type addValue) { name  result(*this); result._flags = addValue; return result; }\
	name&   operator |=(type addValue) { _flags |= addValue; return *this; }\
	name    operator |(type addValue) { name  result(*this); result |= addValue; return result; }\
	name&   operator &=(type maskValue) { _flags &= maskValue; return *this; }\
	name    operator &(type maskValue) { name  result(*this); result &= maskValue; return result; }\
	name    operator ~() { name  result(*this); result._flags = ~result._flags; return result; }\
	type Count() { type count = 0; for (type i = 0; i < sizeof(type); i++) { count += _flags << i; } }\
	operator type() { return _flags; }\
\
	unsigned int _flags;\
};\
}



#endif