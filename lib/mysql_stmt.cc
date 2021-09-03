#include "mysql_stmt.h"
#include <string>
#include <string.h>
#include "third-party/TinySTL/include/utility.h"
#include "third-party/TinySTL/include/config.h"
#include "mysql_exception.h"

namespace zxy {

PreparedStmt::PreparedStmt(MYSQL* connection, TinySTL::string_view query)
	: stmt_(mysql_stmt_init(connection))
{
	if(!stmt_){
		throw MysqlException{"mysql_stmt_init: Out of memory"};
	}

	if(mysql_stmt_prepare(stmt_, query.data(), query.size())){
		std::string error = MysqlException::GetServerError(stmt_);

		// because reset not clear
		if(mysql_stmt_free_result(stmt_))
			error += "\n failed in mysql_stmt_free_result() ";
		if(mysql_stmt_close(stmt_))
			error += "\n failed in mysql_stme_close()";

		throw MysqlException{error};
	}

	// cache count
	parameter_count_ = mysql_stmt_param_count(stmt_);
	field_count_ = mysql_stmt_field_count(stmt_);
}

// just print error message to stderr
// destructor must don't throw exception
PreparedStmt::~PreparedStmt()
{
	if(!stmt_)
		return ;

	if(mysql_stmt_free_result(stmt_) != 0){
		perror("failed in mysql_stmt_free_result(destructor)");
	}

	if(mysql_stmt_close(stmt_) != 0)
		perror("failed in mysql_stmt_close(destructor)");

}

PreparedStmt::PreparedStmt(PreparedStmt&& rhs) noexcept
	: stmt_(rhs.stmt_)
	, parameter_count_(rhs.parameter_count_)
	, field_count_(rhs.field_count_)
{ rhs.stmt_ = nullptr; }

PreparedStmt& PreparedStmt::operator=(PreparedStmt&& rhs) noexcept 
{
	this->swap(rhs);
	return *this;
}

int PreparedStmt::Execute() const noexcept
{
	 return mysql_stmt_execute(stmt_);
}

bool PreparedStmt::Close() noexcept
{
	bool ret = mysql_stmt_close(stmt_); 
	stmt_ = nullptr;
	return ret;
}

int PreparedStmt::Fetch() const noexcept
{
	return mysql_stmt_fetch(stmt_); 
}

bool PreparedStmt::BindResult(MYSQL_BIND* binds) const noexcept
{ 
	return mysql_stmt_bind_result(stmt_, binds);
}

int PreparedStmt::StoreEntireResult() const noexcept
{
	return mysql_stmt_store_result(stmt_);
}

void PreparedStmt::swap(PreparedStmt& rhs) noexcept
{
	STL_SWAP(stmt_, rhs.stmt_);	
	STL_SWAP(parameter_count_, rhs.parameter_count_);
	STL_SWAP(field_count_, rhs.field_count_);
}

} // namespace zxy
