#include "mysql.h"
#include <mysql/mysql.h>
#include <stdio.h>
#include "mysql_exception.h"

namespace zxy{
	Mysql::Mysql(
			char const* host,
			char const* user,
			char const* password,
			char const* db,
			uint16_t port)
		: connection_(mysql_init(NULL)){
		//mysql_init is not thread-safe
		//so you should call mysql_library_init() before the start to any thread
	
		if(! connection_){
			char buf[256];
			snprintf(buf, sizeof(buf), "Unable to connect to Mysql %s", db);
			throw MysqlException{buf};
		}

		MYSQL* const flag = 
			mysql_real_connect(
				connection_,
				host,
				user,
				password,
				db,
				port, NULL, 0);

		if(!flag){
			std::string error(MysqlException::GetServerError(flag));
			
			MysqlException err{error + "(occurred in Mysql constructor)"};
			mysql_close(connection_);
			throw err;
		}
	}

	Mysql::Mysql(
		char const* host,
		char const* user,
		char const* password,
		uint16_t port)
		: Mysql(host, user, password, NULL, port)
	{ }


} //namespace zxy


