#ifndef _ZXY_MYSQL_H
#define _ZXY_MYSQL_H

#include <mysql/mysql.h>
#include "helper.h"
#include "util/noncopyable.h"
#include "mysql_exception.h"
#include "mysql_stmt.h"
#include "type_def.h"
#include "input_binder.h"
#include "output_binder.h"
#include "third-party/TinySTL/include/string_view.h"
#include <mysql/mysqld_error.h>


namespace zxy{
	
class Database : noncopyable {
public:
	Database(
		char const* host,
		char const* user,
		char const* password,
		char const* db,
		uint16_t port = 3306
	);

	Database(
		char const* host,
		char const* user,
		char const* password,
		uint16_t port = 3306
	);

	~Database();
	
	Database(Database&&) noexcept;
	Database& operator=(Database&&) noexcept;
	
	int SetCharacterSet(char const* csname)
	{ return mysql_set_character_set(connection_, csname); }

	/**
	 * @brief run modified command : INSERT UPDATE DELETE
	 * @tparam Args type of data
	 * @param stmt Mysql prepared statement
	 * @param args data content
	 * @return the number of affected rows
	 */
	template<typename ... Args>
	uint64_t RunModification(PreparedStmt const& stmt, Args const&... args);
	

	template<typename ...Args>
	uint64_t RunModification(TinySTL::string_view query, Args const&... args)
	{ return RunModification(GetPreparedStmt(query), args...); }
	
	uint64_t RunModification(TinySTL::string_view query);

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
	void RunQuery(TupleVector<OutputArgs...>& result, TinySTL::string_view query, InputArgs&... args)
	{ RunQuery(result, GetPreparedStmt(query), args...); }

// use this interface get prepared statement
	PreparedStmt GetPreparedStmt(TinySTL::string_view query) const
	{ return PreparedStmt{ connection_, query }; }

// get
	MYSQL* connection() const noexcept { return connection_; }

// state
	bool IsTableExists() const noexcept 
	{ return saved_errno_ == ER_TABLE_EXISTS_ERROR; }

private:
	MYSQL* connection_;

	decltype(mysql_errno(connection_)) saved_errno_;
};

template<typename ...Args>
uint64_t Database::RunModification(PreparedStmt const& stmt, Args const&... args){
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
		saved_errno_ = mysql_errno(connection_);
		if (saved_errno_ != ER_TABLE_EXISTS_ERROR)
			throw MysqlException{ error + "(occurred in call to mysql_stmt_execute())" };
		else {
			return -1;
		}
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
void Database::RunQuery(TupleVector<OutputArgs...>& result, PreparedStmt const& stmt, InputArgs& ...args){
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
