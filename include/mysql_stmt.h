#ifndef _ZXY_MYSQL_STMT_H
#define _ZXY_MYSQL_STMT_H

#include "util/noncopyable.h"
#include "third-party/TinySTL/include/string_view.h"
#include <mysql/mysql.h>

namespace zxy {

class PreparedStmt : noncopyable
{
public:
	using MysqlStmtParameteCount = unsigned long;
	using MysqlStmtFieldCount = unsigned int;

public:
	PreparedStmt()
		: stmt_(NULL), parameter_count_(0), field_count_(0) 
	{ }
	
	PreparedStmt(MYSQL* connection, TinySTL::string_view query);
	~PreparedStmt();
	
	PreparedStmt(PreparedStmt&& rhs) noexcept;

	PreparedStmt& operator= (PreparedStmt&& rhs) noexcept;
	
		 
	MysqlStmtParameteCount parameter_count() const noexcept { return parameter_count_; }

	MysqlStmtFieldCount field_count() const noexcept { return field_count_; }

	MYSQL_STMT* stmt() const noexcept { return stmt_; }
	
		
	int Prepare(TinySTL::string_view);
	// error: 
	// worse order execute command
	// out of memory
	// server gone
	// unknown
	int Execute() const noexcept;

	// error: server gone or unknown error
	bool Close() noexcept;
	
	
	bool BindResult(MYSQL_BIND* bind) const noexcept;

	// after call to Execute() because it need result set exists
	// using buffer: BindResult
	int Fetch() const noexcept;
	
	// note not clear the bindings and stored sets 
	//bool Reset() noexcept;

	int StoreEntireResult() const noexcept;

	void swap(PreparedStmt& rhs) noexcept;

	// after execuate INSERT and 
	// inserted table contains AUTO_INCREMENT field
	uint64_t GetInsertId() const noexcept
	{ return mysql_stmt_insert_id(stmt_); }
	
	// may add:	
	// fetch column(implemetation refetch truncated column)
	// rows of num
	// row seek
	// row tell
	
	operator bool() const noexcept
	{ return stmt_ != NULL; }
private:
	MYSQL_STMT* stmt_;

	MysqlStmtParameteCount parameter_count_;
	MysqlStmtFieldCount field_count_;
};

} // namespacec zxy

#endif // _ZXY_MYSQL_STMT_H
