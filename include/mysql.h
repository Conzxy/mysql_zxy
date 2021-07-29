#ifndef _ZXY_MYSQL_H
#define _ZXY_MYSQL_H

#include <mysql/my_command.h>
#include <mysql/mysql.h>
#include "helper.h"
#include "stl_utility.h"
#include "util/noncopyable.h"
#include "mysql_exception.h"
#include "mysql_stmt.h"
#include "type_def.h"
#include "input_binder.h"
#include "output_binder.h"


namespace zxy{
	
class Mysql : noncopyable {
public:
	Mysql(
		char const* host,
		char const* user,
		char const* password,
		char const* db,
		uint16_t port = 3306
	);

	Mysql(
		char const* host,
		char const* user,
		char const* password,
		uint16_t port = 3306
	);

	~Mysql() { 
		mysql_close(connection_);
	}

	/**
	 * @brief run modified command : INSERT UPDATE DELETE
	 * @tparam Args type of data
	 * @param stmt Mysql prepared statement
	 * @param args data content
	 * @return the number of affected rows
	 */
	template<typename ... Args>
	uint64_t RunModification(PreparedStmt const& stmt, Args&... args);
	

	template<typename ...Args>
	uint64_t RunModification(char const* query, Args&... args)
	{ return RunModification(PreparedStmt(connection_, query), args...); }
	
	/**
	 * @brief run query command : SELECT
	 * @tparam InputArgs "?" parameter type
	 * @tparam OutputArgs field type in result set
	 * @param result output reselt set
	 * @param stmt mysql prepared statement
	 * @param args input parameters
	 * @return void
	 */
	template<typename ...InputArgs, typename ...OutputArgs>
	void RunQuery(TupleVector<OutputArgs...>& result, PreparedStmt const& stmt,  InputArgs&... args);
	
	template<typename ...InputArgs, typename ...OutputArgs>
	void RunQuery(TupleVector<OutputArgs...>& result, char const* query, InputArgs&... args)
	{ RunQuery(result, PreparedStmt(connection_, query), args...); }
	
	PreparedStmt GetPreparedStmt(char const* query) const
	{ return PreparedStmt{ connection_, query }; }

	PreparedStmt GetPreparedStmt(std::string const& query) const
	{ return GetPreparedStmt(query.c_str()); }

	MYSQL* connection() const noexcept { return connection_; }
private:
	MYSQL* connection_;
};

template<typename ...Args>
uint64_t Mysql::RunModification(PreparedStmt const& stmt, Args&... args){
	if(stmt.field_count() != 0)
		throw MysqlException{"Tried to run query with RunModification(You should call RunQuery()"};

	//bind input parameters
	helper::ThrowIfNotRequiredParameterCount(stmt, sizeof...(Args));
	MysqlBindVector parameters(stmt.parameter_count());
	BindInputs(parameters, args...);

	if(mysql_stmt_bind_param(stmt.stmt(), parameters.data())){
		std::string error = MysqlException::GetServerError(stmt.stmt());
		throw MysqlException{ error + "(occurred in call to mysql_stmt_bind_param())" };
	}

	//exec
	if(mysql_stmt_execute(stmt.stmt())){
		std::string error = MysqlException::GetServerError(stmt.stmt());
		throw MysqlException{ error + "(occurred in call to mysql_stmt_execute())" };
	}
	
	//check affected rows
	const auto affected_rows = mysql_stmt_affected_rows(stmt.stmt());
	if(affected_rows == static_cast<decltype(affected_rows)>(-1)){
		//error occured or SELECT
		throw MysqlException{"Error occurred or Tried to query with RunModification()"
			"(occurred in call to mysql_stmt_affected_rows())"};
	}
	
	return affected_rows;

}

template<typename ...InputArgs, typename ...OutputArgs>
void Mysql::RunQuery(TupleVector<OutputArgs...>& result, PreparedStmt const& stmt, InputArgs& ...args){
	if(stmt.field_count() == 0)
		throw MysqlException{"Tried to run Modification with RunQuery()"};
	
	helper::ThrowIfNotRequiredParameterCount(stmt, sizeof...(InputArgs));

	MysqlBindVector input_parameters(sizeof...(args));
	BindInputs(input_parameters, args...);
	if(mysql_stmt_bind_param(stmt.stmt(), input_parameters.data())){
		std::string error = MysqlException::GetServerError(stmt.stmt());
		throw MysqlException{ error + "(occurred in call to mysql_stmt_bind_param)" };
	}
	
	SetResultset(result, stmt);
}

} //namespace zxy

#endif //_ZXY_MYSQL_H
