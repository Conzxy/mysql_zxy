#ifndef _ZXY_MYSQL_TYPE_DEF_H
#define _ZXY_MYSQL_TYPE_DEF_H

#include <bits/c++config.h>
#include <mysql/mysql_time.h>
#include <stdint.h>
#include <vector>
#include <deque>
#include <mysql/mysql.h>
#include "third-party/TinySTL/include/tuple.h"
#include <memory>

namespace zxy{

constexpr int kTBlobSize = 1 << 8; //256B
constexpr int kBlobSize = 1 << 16; //64K
constexpr int kMBlobSize = 1 << 24; //16M
constexpr int64_t kLBlobSize = static_cast<int64_t>(1) << 32; //4G;

struct MysqlTimeBuilder;

//make_unique since c++14
template<typename T, typename ...Args>
std::unique_ptr<T> MakeUnique(Args&&... args) 
{ return std::unique_ptr<T>(new T(STL_FORWARD(Args, args)...)); }

#define DEFINE_SQL_TIME(name) \
struct name {  \
	using self = name; \
public:  \
	MYSQL_TIME& data() noexcept \
	{ return data_; } \
	 \
	MYSQL_TIME const& data() const noexcept \
	{ return data_; } \
 \
	name(MYSQL_TIME const& time)\
		: data_{ time }\
	{ }\
\
	static std::unique_ptr<MysqlTimeBuilder> Build() \
	{ return MakeUnique<MysqlTimeBuilder>(); } \
private: \
	MYSQL_TIME data_;  \
};

struct MysqlTimeBuilder { 
	using self = MysqlTimeBuilder;

	MYSQL_TIME time_;

	MysqlTimeBuilder()
	{ } 

	self& Year(unsigned int y) 
	{ time_.year = y; return *this; } 
                                                                           
	self& Month(unsigned int m)
	{ time_.month = m; return *this; } 
 
	self&  Day(unsigned int d) 
	{ time_.day = d; return *this; } 
	
	self& Hour(unsigned int h)
	{ time_.hour = h; return *this; } 

	self& Minute(unsigned int min)
	{ time_.minute = min; return *this; }

	self& Second(unsigned int second)
	{ time_.second = second; return *this; }
 
}; 

DEFINE_SQL_TIME(Date)
DEFINE_SQL_TIME(DateTime)
DEFINE_SQL_TIME(Timestamp)
DEFINE_SQL_TIME(Time)

template<int64_t N>
struct SQLBlob{
	char data[N];
};

using TinyBlob = SQLBlob<kTBlobSize>;
using MediumBlob = SQLBlob<kMBlobSize>;
using Blob = SQLBlob<kBlobSize>;
using LongBlob = SQLBlob<kLBlobSize>;

using MysqlOff = unsigned long;
using MysqlCol = unsigned int;

using MysqlBuffer = std::vector<char>;

using MysqlBindVector = std::vector<MYSQL_BIND>;

using TinySTL::Tuple;
using TinySTL::Get;

template<typename ...Args> using TupleVector = std::vector<Tuple<Args...>>;
using MysqlBufferVector = std::vector<MysqlBuffer>;
using MysqlIsNullVector = std::deque<bool>; //std::vector<bool> is not a real container
using MysqlOffVector = std::vector<MysqlOff>;
using MysqlColVector = std::vector<MysqlCol>;
using MysqlColOffMap = std::vector<TinySTL::pair<MysqlCol, MysqlOff>>;

} //namespace zxy

#endif //_ZXY_MYSQL_DEF_H
