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
REGISTER_TYPE(long long, MYSQL_TYPE_LONGLONG)
REGISTER_TYPE(unsigned long long, MYSQL_TYPE_LONGLONG)

REGISTER_TYPE(std::string, MYSQL_TYPE_VAR_STRING)

REGISTER_TYPE(char*, MYSQL_TYPE_STRING)
REGISTER_TYPE(char const*, MYSQL_TYPE_STRING)

template<unsigned N>
REGISTER_TYPE(char[N], MYSQL_TYPE_STRING)
template<unsigned N>
REGISTER_TYPE(char const[N], MYSQL_TYPE_STRING)

REGISTER_TYPE(float, MYSQL_TYPE_FLOAT)
REGISTER_TYPE(double, MYSQL_TYPE_DOUBLE)

REGISTER_TYPE(Date, MYSQL_TYPE_DATE)
REGISTER_TYPE(DateTime, MYSQL_TYPE_DATETIME)
REGISTER_TYPE(Time, MYSQL_TYPE_TIME)
REGISTER_TYPE(Timestamp, MYSQL_TYPE_TIMESTAMP)

REGISTER_TYPE(Blob, MYSQL_TYPE_BLOB)
REGISTER_TYPE(TinyBlob, MYSQL_TYPE_TINY_BLOB)
REGISTER_TYPE(MediumBlob, MYSQL_TYPE_MEDIUM_BLOB)
REGISTER_TYPE(LongBlob, MYSQL_TYPE_LONG_BLOB)

using TinySTL::_true_type;
using TinySTL::_false_type;
using TinySTL::Void_t;

// use SFINAE partial specialization detect given type if registered type
template<typename T, typename = void>
struct IsMysqlType : _false_type {};

template<typename T>
struct IsMysqlType<T, Void_t<decltype(TypeMap(TypeIdentity<T>{}))>> 
	: _true_type {};

} //namespace zxy

#endif //_ZXY_MYSQL_TYPE_MAP_H
