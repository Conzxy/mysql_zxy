#ifndef _ZXY_MYSQL_STMT_H
#define _ZXY_MYSQL_STMT_H

#include "util/noncopyable.h"
#include <mysql/mysql.h>

namespace zxy {

class PreparedStmt : noncopyable
{
public:
	using MysqlStmtParameteCount = unsigned long;
	using MysqlStmtFieldCount = unsigned int;

public:
	PreparedStmt(MYSQL* connection, char const* query);
	~PreparedStmt();
	
	PreparedStmt(PreparedStmt&& rhs) noexcept;

	PreparedStmt& operator= (PreparedStmt&& rhs) noexcept;

	MysqlStmtParameteCount parameter_count() const noexcept { return parameter_count_; }

	MysqlStmtFieldCount field_count() const noexcept { return field_count_; }

	MYSQL_STMT* stmt() const noexcept { return stmt_; }

	void swap(PreparedStmt& rhs) noexcept;

private:
	MYSQL_STMT* stmt_;

	MysqlStmtParameteCount parameter_count_;
	MysqlStmtFieldCount field_count_;
};

} // namespacec zxy

#endif // _ZXY_MYSQL_STMT_H
