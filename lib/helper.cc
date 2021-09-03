#include "helper.h"
#include "mysql_stmt.h"
#include "mysql_exception.h"
#include "mysql_stmt.h"
#include <string>
#include <stdio.h>

namespace zxy {

namespace helper {

void BindResultAndExec(PreparedStmt const& stmt, MysqlBindVector& binds){
	if(stmt.BindResult(binds.data()) != 0)
		throw MysqlException{stmt.stmt()};

	if(stmt.Execute() != 0)
		throw MysqlException{stmt.stmt()};
}

void ThrowIfNotRequiredFieldCount(PreparedStmt const& stmt, unsigned int fieldcount){
	if(stmt.field_count() != fieldcount){
		char buf[256];
		snprintf(buf, sizeof(buf), "Required output parameters is %u,"
				"but given output paramters is %u\n",
				stmt.field_count(),
				fieldcount);

		throw MysqlException{buf};
	}
}

void ThrowIfNotRequiredParameterCount(PreparedStmt const& stmt, unsigned long parametercount){
	if(stmt.parameter_count() != parametercount){
		char buf[256];
		snprintf(buf ,sizeof(buf), "Required output parameters is %lu,"
				"buf given output parameters is %lu\n",
				stmt.parameter_count(),
				parametercount);

		throw MysqlException{buf};
	}
}

void ThrowIfFetchFialed(PreparedStmt const& stmt, int fetchret){
	switch(fetchret){
		case MYSQL_NO_DATA:
			break;
		case 1:
			throw MysqlException{stmt.stmt()};
		default:
			throw MysqlException{"Unknown error for fetch failed"};
	}
}

// may have more than 2 rows truncated
void ReFetchTruncCol(
	PreparedStmt const& stmt, 
	MysqlBindVector& binds,
	MysqlBufferVector& buffers,
	MysqlBindLengthVector& lengths)
{
	MysqlColOffMap col_offs;	
	for(size_t i = 0; i != binds.size(); ++i){
		//MYSQL_BIND.length indicates untrunctcated length after fetch
		//and min{length, buffer_length} indicates actual length 
		auto untruncated_length = lengths[i];
		auto actual_length = binds[i].buffer_length;

		if(untruncated_length > actual_length){
			buffers[i].resize(untruncated_length + 1);
			col_offs.emplace_back(i, actual_length);
			binds[i].buffer = &buffers[i][actual_length];
			//need check 
			binds[i].buffer_length = untruncated_length - actual_length;
		}
	
		if(col_offs.empty())
			return ;

		//refetch column
		for(auto& col_off : col_offs){
			auto col = col_off.first;
			auto off = col_off.second;
			
			auto& bind = binds[col];
			if(mysql_stmt_fetch_column(stmt.stmt(), &bind, col, off) != 0)
				throw MysqlException{"failed in refetch truncated columns"};
			
			//reset buffer to rebind
			auto& buffer = buffers[col];
			bind.buffer = buffer.data();
			bind.buffer_length = buffer.size();

		}

		if(mysql_stmt_bind_result(stmt.stmt(), binds.data()) != 0)
			throw MysqlException{"fail in rebind(refetch truncated colums)"};
		
	}
}

} // namespace helper
} // namespace zxy

