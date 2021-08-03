#include "mysql_stmt.h"
#include <string>
#include <string.h>
#include "utility.h"
#include "config.h"
#include "mysql_exception.h"

namespace zxy {

PreparedStmt::PreparedStmt(MYSQL* connection, char const* query)
	: stmt_(mysql_stmt_init(connection))
{
	if(! stmt_){
		throw MysqlException{"mysql_stmt_init: Out of memory"};
	}

	if(mysql_stmt_prepare(stmt_, query, strlen(query))){
		std::string error = MysqlException::GetServerError(stmt_);

		if(mysql_stmt_free_result(stmt_))
			error += "\n failed in mysql_stmt_free_result() ";
		if(mysql_stmt_close(stmt_))
			error += "\n failed in mysql_stme_close()";

		throw MysqlException{error};
	}

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

void PreparedStmt::swap(PreparedStmt& rhs) noexcept
{
	std::swap(stmt_, rhs.stmt_);	
	std::swap(parameter_count_, rhs.parameter_count_);
	std::swap(field_count_, rhs.field_count_);
}

} // namespace zxy
