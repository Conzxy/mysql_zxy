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
#include "util/noncopyable.h"
#include <time.h>
#include <string.h>

namespace zxy{

constexpr int kTBlobSize = 1 << 8; //256B
constexpr int kBlobSize = 1 << 16; //64K
constexpr int kMBlobSize = 1 << 24; //16M
constexpr int64_t kLBlobSize = static_cast<int64_t>(1) << 32; //4G;


//make_unique since c++14
template<typename T, typename ...Args>
std::unique_ptr<T> MakeUnique(Args&&... args) 
{ return std::unique_ptr<T>(new T(STL_FORWARD(Args, args)...)); }

class MysqlTime : public MYSQL_TIME
{
public:
	MysqlTime() = default;
	~MysqlTime() = default;
	
	MysqlTime(const struct tm& time)
	{
		year = time.tm_year + 1900;
		month = time.tm_mon + 1;
		day = time.tm_mday;
		hour = time.tm_hour;
		minute = time.tm_min;
		second = time.tm_sec;
		// second_part must be specified if buffer_type is DATETIME, TIMESTAMP
		second_part = 0;
		time_type = MYSQL_TIMESTAMP_DATETIME;
	}


	MysqlTime(time_t value)
	{
		struct tm time;
		localtime_r(&value, &time);
		new(this) MysqlTime(time);
	}
	
	static MysqlTime now()
	{
		time_t value;
		::time(&value);
		return MysqlTime(value);
	}
	
	time_t time() const
	{
		struct tm time;
		time.tm_year = year - 1900;
		time.tm_mon = month - 1;
		time.tm_mday = day;
		time.tm_min = minute;
		time.tm_sec = second;
		return ::mktime(&time);
	}
};

template<uint64_t N>
struct SQLBlob{
	char data[N];
};

using TinyBlob = SQLBlob<kTBlobSize>;
using MediumBlob = SQLBlob<kMBlobSize>;
using Blob = SQLBlob<kBlobSize>;
using LongBlob = SQLBlob<kLBlobSize>;

using MysqlOff = unsigned long;
using MysqlCol = unsigned int;
using MysqlBindLength = unsigned long;

using MysqlBuffer = std::vector<char>;
using MysqlBindLengthVector = std::vector<MysqlBindLength>;
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
