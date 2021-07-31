#ifndef _ZXY_MYSQL_TYPE_MAP_H
#define _ZXY_MYSQL_TYPE_MAP_H

#include <mysql/field_types.h>
#include <mysql/mysql.h>
#include <mysql/mysql_com.h>
#include <string>
#include "type_def.h"
#include "stl_type_traits_base.h"

namespace zxy{

template<typename T>
struct TypeIdentity{
	using type = T;
};

#define REGISTER_TYPE(type, mysql_type) \
constexpr auto TypeMap(TypeIdentity<type>) \
	 ->decltype(mysql_type) { \
	return mysql_type; \
}

REGISTER_TYPE(char, MYSQL_TYPE_TINY)
REGISTER_TYPE(unsigned char, MYSQL_TYPE_TINY)

REGISTER_TYPE(short, MYSQL_TYPE_SHORT)
REGISTER_TYPE(unsigned short, MYSQL_TYPE_SHORT)

REGISTER_TYPE(int, MYSQL_TYPE_LONG)
REGISTER_TYPE(unsigned int, MYSQL_TYPE_LONG)

REGISTER_TYPE(int64_t, MYSQL_TYPE_LONGLONG)
REGISTER_TYPE(uint64_t, MYSQL_TYPE_LONGLONG)

REGISTER_TYPE(std::string, MYSQL_TYPE_VAR_STRING)

REGISTER_TYPE(char*, MYSQL_TYPE_STRING)
REGISTER_TYPE(char const*, MYSQL_TYPE_STRING)

template<unsigned N>
REGISTER_TYPE(char[N], MYSQL_TYPE_STRING)
template<unsigned N>
REGISTER_TYPE(char const[N], MYSQL_TYPE_STRING)

REGISTER_TYPE(float, MYSQL_TYPE_FLOAT)
REGISTER_TYPE(double, MYSQL_TYPE_DOUBLE)

REGISTER_TYPE(Blob, MYSQL_TYPE_BLOB)
REGISTER_TYPE(TinyBlob, MYSQL_TYPE_TINY_BLOB)
REGISTER_TYPE(MediumBlob, MYSQL_TYPE_MEDIUM_BLOB)
REGISTER_TYPE(LongBlob, MYSQL_TYPE_LONG_BLOB)

using TinySTL::_true_type;
using TinySTL::_false_type;

#if __cplusplus >= 201402L
template<typename T>
constexpr bool IsMysqlType_v = false;

#define IsMysqlType_ARRAY_SPECILIZATION(type) \
template<unsigned N>  \
constexpr bool IsMysqlType_v<type[N]> = true; 

#define IsMysqlType_FULL_SPECILIZATION(type) \
template<> \
constexpr bool IsMysqlType_v<type> = true; 
#else // c++11

template<typename T>
struct IsMysqlType : _false_type {};

#define IsMysqlType_ARRAY_SPECILIZATION(type) \
template<unsigned N> \
struct IsMysqlType<type[N]> : _true_type {};

#define IsMysqlType_FULL_SPECILIZATION(type) \
template<> \
struct IsMysqlType<type> : _true_type {};

#endif // __cplusplus >= 201402L

IsMysqlType_ARRAY_SPECILIZATION(char)
IsMysqlType_ARRAY_SPECILIZATION(char const)

IsMysqlType_FULL_SPECILIZATION(char)
IsMysqlType_FULL_SPECILIZATION(unsigned char)
IsMysqlType_FULL_SPECILIZATION(short)
IsMysqlType_FULL_SPECILIZATION(unsigned short)
IsMysqlType_FULL_SPECILIZATION(int)
IsMysqlType_FULL_SPECILIZATION(unsigned int)
IsMysqlType_FULL_SPECILIZATION(int64_t)
IsMysqlType_FULL_SPECILIZATION(uint64_t)
IsMysqlType_FULL_SPECILIZATION(std::string)
IsMysqlType_FULL_SPECILIZATION(char*)
IsMysqlType_FULL_SPECILIZATION(char const*)
IsMysqlType_FULL_SPECILIZATION(float)
IsMysqlType_FULL_SPECILIZATION(double)
IsMysqlType_FULL_SPECILIZATION(Blob)
IsMysqlType_FULL_SPECILIZATION(TinyBlob)
IsMysqlType_FULL_SPECILIZATION(MediumBlob)
IsMysqlType_FULL_SPECILIZATION(LongBlob)

#if __cplusplus >= 201402L
	template<typename T>
	using IsMysqlType = TinySTL::Bool_constant<IsMysqlType_v<T>>;
#endif

} //namespace zxy

#endif //_ZXY_MYSQL_TYPE_MAP_H
