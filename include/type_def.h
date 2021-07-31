#ifndef _ZXY_MYSQL_TYPE_DEF_H
#define _ZXY_MYSQL_TYPE_DEF_H

#include <stdint.h>
#include <vector>
#include <deque>
#include <mysql/mysql.h>
#include "tuple.h"

namespace zxy{

constexpr int kTBlobSize = 1 << 8; //256B
constexpr int kBlobSize = 1 << 16; //64K
constexpr int kMBlobSize = 1 << 24; //16M
constexpr int64_t kLBlobSize = static_cast<int64_t>(1) << 32; //4G;

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
template<typename ...Args> using TupleVector = std::vector<TinySTL::Tuple<Args...>>;
using MysqlBufferVector = std::vector<MysqlBuffer>;
using MysqlIsNullVector = std::deque<bool>; //std::vector<bool> is not a real container
using MysqlOffVector = std::vector<MysqlOff>;
using MysqlColVector = std::vector<MysqlCol>;
using MysqlColOffMap = std::vector<TinySTL::pair<MysqlCol, MysqlOff>>;

} //namespace zxy

#endif //_ZXY_MYSQL_DEF_H
