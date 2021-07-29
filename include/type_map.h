#ifndef _ZXY_MYSQL_TYPE_MAP_H
#define _ZXY_MYSQL_TYPE_MAP_H

#include <mysql/field_types.h>
#include <mysql/mysql.h>
#include <mysql/mysql_com.h>
#include <string>
#include "type_def.h"

namespace zxy{

template<typename T>
struct TypeIdentity{
	using type = T;
};

#define REGISTER_TYPE(type, mysql_type) \
constexpr auto TypeMap(TypeIdentity<type>){ \
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

template<typename T>
inline constexpr bool kIsMysqlType = false;

#define kIsMysqlType_ARRAY_SPECILIZATION(type) \
template<unsigned N>  \
inline constexpr bool kIsMysqlType<type[N]> = true; 

kIsMysqlType_ARRAY_SPECILIZATION(char)
kIsMysqlType_ARRAY_SPECILIZATION(char const)

#define kIsMysqlType_FULL_SPECILIZATION(type) \
template<> \
inline constexpr bool kIsMysqlType<type> = true; 

kIsMysqlType_FULL_SPECILIZATION(char)
kIsMysqlType_FULL_SPECILIZATION(unsigned char)
kIsMysqlType_FULL_SPECILIZATION(short)
kIsMysqlType_FULL_SPECILIZATION(unsigned short)
kIsMysqlType_FULL_SPECILIZATION(int)
kIsMysqlType_FULL_SPECILIZATION(unsigned int)
kIsMysqlType_FULL_SPECILIZATION(int64_t)
kIsMysqlType_FULL_SPECILIZATION(uint64_t)
kIsMysqlType_FULL_SPECILIZATION(std::string)
kIsMysqlType_FULL_SPECILIZATION(char*)
kIsMysqlType_FULL_SPECILIZATION(char const*)
kIsMysqlType_FULL_SPECILIZATION(float)
kIsMysqlType_FULL_SPECILIZATION(double)
kIsMysqlType_FULL_SPECILIZATION(Blob)
kIsMysqlType_FULL_SPECILIZATION(TinyBlob)
kIsMysqlType_FULL_SPECILIZATION(MediumBlob)
kIsMysqlType_FULL_SPECILIZATION(LongBlob)

} //namespace zxy

#endif //_ZXY_MYSQL_TYPE_MAP_H
