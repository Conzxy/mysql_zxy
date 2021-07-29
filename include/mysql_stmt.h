#ifndef _ZXY_MYSQL_STMT_H
#define _ZXY_MYSQL_STMT_H

#include "util/noncopyable.h"
#include <mysql/mysql.h>
#include "mysql_exception.h"
#include <string.h>
#include <string>

namespace zxy {

class PreparedStmt : noncopyable {
public:
	PreparedStmt(MYSQL* connection, char const* query)
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
	
	//just print error message to stderr
	//destructor must don't throw exception
	~PreparedStmt(){
		if(mysql_stmt_free_result(stmt_) != 0){
			perror("failed in mysql_stmt_free_result(destructor)");
		}

		if(mysql_stmt_close(stmt_) != 0)
			perror("failed in mysql_stmt_close(destructor)");
	}

	auto parameter_count() const noexcept { return parameter_count_; }
	auto field_count() const noexcept { return field_count_; }
	auto stmt() const noexcept { return stmt_; }
private:
	MYSQL_STMT* stmt_;

	unsigned long parameter_count_;
	unsigned int field_count_;
};

} // namespacec zxy

#endif // _ZXY_MYSQL_STMT_H
