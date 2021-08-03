#ifndef _ZXY_MYSQL_HELPER_H
#define _ZXY_MYSQL_HELPER_H

#include "mysql_stmt.h"
#include "type_def.h"

namespace zxy{

class PreparedStmt;
namespace helper {

void BindResultAndExec(PreparedStmt const& stmt, MysqlBindVector& binds);

void ThrowIfNotRequiredFieldCount(PreparedStmt const& stmt, unsigned int fieldcount);

void ThrowIfNotRequiredParameterCount(PreparedStmt const& stmt, unsigned long parametercount);

void ThrowIfFetchFialed(PreparedStmt const& stmt, int fetchret);

int Fetch(PreparedStmt const& stmt);

void ReFetchTruncCol(
		PreparedStmt const& stmt, 
		MysqlBindVector& binds,
		MysqlBufferVector& buffers
		);

} // namespace helper
} // namespace zxy

#endif // _ZXY_MYSQL_HELPER_H
