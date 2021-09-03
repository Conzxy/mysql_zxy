#ifndef _ZXY_MYSQL_EXCEPTION
#define _ZXY_MYSQL_EXCEPTION

#include <mysql/mysql.h>
#include <string>
#include <string.h>

namespace zxy{

class MysqlException{
public:
	explicit MysqlException(char const* msg)
		: msg_(msg)
	{ }
	
	explicit MysqlException(std::string const& str)
		: msg_(str)
	{ }

	explicit MysqlException(MYSQL* connection)
		: msg_(GetServerError(connection))
	{ }
	
	explicit MysqlException(MYSQL_STMT* stmt)
		: msg_(GetServerError(stmt))
	{ }
	
	char const* what() const noexcept { return msg_.c_str(); }

	static char const* GetServerError(MYSQL const* const connection){
		char const* error = mysql_error(const_cast<MYSQL*>(connection));
		if(error[0] != '\0')
			return error;

		return "Unknown error";
	}
	
	static char const* GetServerError(MYSQL_STMT const* const stmt){
		char const* error = mysql_stmt_error(const_cast<MYSQL_STMT*>(stmt));

		if(error[0] != 0)
			return error;

		return "Unknown error";
	}

private:
	std::string msg_;
};

} // namespace zxy

#endif // _ZXY_MYSQL_EXCEPTION
