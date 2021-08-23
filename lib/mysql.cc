#include "mysql.h"
#include <mysql/mysql.h>
#include <stdio.h>
#include "mysql_exception.h"
#include "third-party/TinySTL/include/config.h"
#include "third-party/TinySTL/include/utility.h"

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

	Mysql::Mysql(Mysql&& other) noexcept
		: connection_{ other.connection_ }
	{ other.connection_ = NULL; }

	Mysql& Mysql::operator=(Mysql&& other) noexcept
	{
		STL_SWAP(this->connection_, other.connection_);
		return *this;
	}

	Mysql::~Mysql()
	{
		if(connection_)
			mysql_close(connection_);
	}
} //namespace zxy


