#include "mysql.h"
#include <mysql/mysql.h>
#include <stdio.h>
#include "mysql_exception.h"
#include "third-party/TinySTL/include/config.h"
#include "third-party/TinySTL/include/utility.h"

namespace zxy{


struct MysqlLibInit
{
	MysqlLibInit()
	{
		// arguments is no effect
		mysql_library_init(0, NULL, NULL);
	}

	~MysqlLibInit()
	{
		mysql_library_end();
	}
};

MysqlLibInit g_mysql_library_init{};

Database::Database(
		char const* host,
		char const* user,
		char const* password,
		char const* db,
		uint16_t port)
	: connection_(mysql_init(NULL)){
	//mysql_init is not thread-safe
	//so you should call mysql_library_init() before the start to any thread

	if(!connection_){
		char buf[256];
		snprintf(buf, sizeof(buf), "Unable to connect to Database %s", db);
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
		
		MysqlException err{error + "(occurred in Database connect to server)"};
		mysql_close(connection_);
		throw err;
	}
}

Database::Database(
	char const* host,
	char const* user,
	char const* password,
	uint16_t port)
	: Database(host, user, password, NULL, port)
{ }

Database::Database(Database&& other) noexcept
	: connection_{ other.connection_ }
{ other.connection_ = NULL; }

Database& Database::operator=(Database&& other) noexcept
{
	STL_SWAP(this->connection_, other.connection_);
	return *this;
}

Database::~Database()
{
	if(connection_)
		mysql_close(connection_);
}

uint64_t Database::RunModification(TinySTL::string_view query)
{
	if(0 != mysql_real_query(connection_, query.data(), query.size()))
	{
		char buf[256];
		strcat(buf, MysqlException::GetServerError(connection_));
		strcat(buf, "occurred in mysql_real_query()");
		throw MysqlException{ buf };
	}

	auto ret = mysql_affected_rows(connection_);
	if(static_cast<uint64_t>(-1) == ret) 
	{
		MYSQL_RES* res = mysql_store_result(connection_);
		mysql_free_result(res);
		throw MysqlException{ "Tired to run Query with RunModification" };
	}

	return ret;
}

} //namespace zxy


