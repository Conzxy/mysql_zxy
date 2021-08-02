#ifndef _ZXY_MYSQL_OUTPUT_BINDER_H
#define _ZXY_MYSQL_OUTPUT_BINDER_H

#include "mysql_stmt.h"
#include "tuple.h"
#include "type_def.h"
#include "type_map.h"
#include "type_traits.h"
#include <mysql/field_types.h>
#include <mysql/mysql.h>
#include <memory>
#include <assert.h>
#include "typelist.h"
#include "mysql_exception.h"
#include "helper.h"

namespace zxy{

template<typename ...Args>
void SetResultset(
		TupleVector<Args...>& tuples,
		PreparedStmt const& stmt);

namespace detail{

/////////////////////////////
// Output Parameters Binder
/////////////////////////////
void BindOutputParameters(
		MysqlBindVector& binds,
		MysqlBufferVector& buffers,
		std::vector<bool>& is_nulls);


template<
	typename Arg, 
	bool is_mysql_type = IsMysqlType<Arg>::value>
struct OutputBinder;

// not mysql required type
template<typename Arg>
struct OutputBinder<Arg, false>{
	static void Apply(MYSQL_BIND& , MysqlBuffer& , bool& ){
		// sizeof(Arg) is always false
		// so this assert must be triggered
		static_assert(sizeof(Arg) < 0, 
				"The type is not valid mysql type"
				"\nplease check give type");
	}
};

// To string(including c-stype, user-defined type)
// char*
template<>
struct OutputBinder<char*, true>{
	static void Apply(MYSQL_BIND& bind, MysqlBuffer& buffer, bool& is_null){
		bind.buffer_type = MYSQL_TYPE_STRING;
		//set a valid buffer size 
		if(buffer.size() == 0)
			buffer.resize(30);
		bind.buffer = buffer.data();
		bind.buffer_length = buffer.size();
		bind.is_null = &is_null;
	}
};

// std::string
template<>
struct OutputBinder<std::string, true>{
	static void Apply(MYSQL_BIND& bind, MysqlBuffer& buffer, bool& is_null){
		bind.buffer_type = MYSQL_TYPE_VAR_STRING;
		if(buffer.size() == 0)
			buffer.resize(30);
		bind.buffer = buffer.data();
		bind.buffer_length = buffer.size();
		bind.is_null = &is_null;
	}
};

template<unsigned SZ>
struct OutputBinder<char[SZ], true>{
	static void Apply(MYSQL_BIND& bind, MysqlBuffer& buffer, bool& is_null){
		bind.buffer_type = MYSQL_TYPE_VAR_STRING;
		if(buffer.size() == 0)
			buffer.resize(SZ);
		bind.buffer = buffer.data();
		bind.buffer_length = buffer.size();
		bind.is_null = &is_null;
	}
};

// other type
template<typename Arg>
struct OutputBinder<Arg, true>{
	static void Apply(MYSQL_BIND& bind, MysqlBuffer&  buffer, bool& is_null){
		bind.buffer_type = TypeMap(TypeIdentity<Arg>{});
		buffer.resize(sizeof(Arg));
		bind.buffer = buffer.data();
		bind.is_null = &is_null;	
		bind.is_unsigned = TinySTL::Is_unsigned<Arg>::value;
	}
};

// prohibit raw pointer
// represent value while may be 'null'
// should use smart pointer(resource safe and no memery leak since RAII tenique)
template<typename Arg>
struct OutputBinder<Arg*, false>{
	static void Apply(MYSQL_BIND&, MysqlBuffer& , bool& ){
		static_assert(sizeof(Arg) < 0,
				"You should set smart pointer(e.g. std::unique_ptr, std::shared_ptr)"
				"because raw pointer may be wild pointer");
	}
};

//smart pointer
#define WRAPPER_OF_OUTPUT_BINDER_FULL_SPECIALIZATION(type)  \
template<typename Arg> \
struct OutputBinder<type<Arg>, false>{ \
	static void Apply(MYSQL_BIND& bind, MysqlBuffer& buffer, bool& is_null){ \
		OutputBinder<Arg>::Apply(bind, buffer, is_null); \
	} \
};

WRAPPER_OF_OUTPUT_BINDER_FULL_SPECIALIZATION(std::unique_ptr)
WRAPPER_OF_OUTPUT_BINDER_FULL_SPECIALIZATION(std::shared_ptr)

using namespace TinySTL::mpl; //Typelist
using namespace TinySTL::mpl::TL; //related algorithm of Typelist

template<int N>
struct IntCounter{};

template<typename T>
void BindOutputParameters(
		MysqlBindVector&,
		MysqlBufferVector&,
		MysqlIsNullVector&,
		IntCounter<-1>)
{ }

template<typename Types, int N>
void BindOutputParameters(
		MysqlBindVector& binds, 
		MysqlBufferVector& buffers, 
		MysqlIsNullVector& is_nulls,
		IntCounter<N>){
		
	OutputBinder<Type_At<Types, N>>::Apply(binds.at(N), buffers.at(N), is_nulls.at(N));
	BindOutputParameters<Types>(binds, buffers, is_nulls, IntCounter<N-1>{});
}

template<typename ...Args>
void BindOutputParameters(
		MysqlBindVector& binds,
		MysqlBufferVector& buffers,
		MysqlIsNullVector& is_nulls){

	assert(binds.size() == buffers.size());
	assert(binds.size() == is_nulls.size());
	assert(buffers.size() == is_nulls.size());

	BindOutputParameters<Typelist<Args...>>(binds, buffers, is_nulls, IntCounter<sizeof...(Args)-1>{});
}

///////////////////////////////
// Result Setter
//////////////////////////////

char const kNullErrorMessage[] = "null value is only allowed to present by smart pointer";

// no need to IsMysqlType
// because call TupleSetter after BindOutputParameters()
template<typename ...Args>
void TupleSetter(TinySTL::Tuple<Args...>& tuple, MysqlBindVector& binds);

template<typename Arg>
void TupleSetterImpl(Arg& arg, MYSQL_BIND& bind){
	if(*bind.is_null)
		throw MysqlException{kNullErrorMessage};

	arg = *reinterpret_cast<Arg*>(bind.buffer);
}

#define WRAPPER_OF_TUPLESETTER_FULL_SPECIALIZATION(type) \
template<typename T> \
void TupleSetterImpl(type<T>& arg, MYSQL_BIND& bind){ \
	if(*bind.is_null) \
		arg.reset(); \
	else \
		arg.reset(new T(*reinterpret_cast<T*>(bind.buffer))); \
}

WRAPPER_OF_TUPLESETTER_FULL_SPECIALIZATION(std::unique_ptr)
WRAPPER_OF_TUPLESETTER_FULL_SPECIALIZATION(std::shared_ptr)

template<>
inline void TupleSetterImpl<std::string>(std::string& str, MYSQL_BIND& bind){
	if(*bind.is_null)
		throw MysqlException{kNullErrorMessage};

	str = reinterpret_cast<char*>(bind.buffer);
}


template<typename ...Args>
void TupleSetter(TinySTL::Tuple<Args...>& , MysqlBindVector& , IntCounter<-1>)
{ }

template<typename ...Args, int N>
void TupleSetter(TinySTL::Tuple<Args...>& tuple, MysqlBindVector& binds, IntCounter<N>){
	//TinySTL::for_each(tuple, TupleSetterImpl);	
	TupleSetterImpl(TinySTL::Get<N>(tuple), binds.at(N));	
	TupleSetter(tuple, binds, IntCounter<N-1>{});
}

template<typename ...Args>
void TupleSetter(TinySTL::Tuple<Args...>& tuple, MysqlBindVector& binds){
	TupleSetter(tuple, binds, IntCounter<sizeof...(Args)-1>{});
}

} // namespace detail

template<typename ...Args>
void SetResultset(TupleVector<Args...>& tuples, PreparedStmt const& stmt){
	helper::ThrowIfNotRequiredFieldCount(stmt, sizeof...(Args));	

	MysqlBindVector binds(stmt.field_count());
	MysqlBufferVector buffers(stmt.field_count());
	MysqlIsNullVector is_nulls(stmt.field_count());
	
	detail::BindOutputParameters<Args...>(binds, buffers, is_nulls);
	helper::BindResultAndExec(stmt, binds);
	auto fetchret = helper::Fetch(stmt);

	while(true){
		if(fetchret == MYSQL_DATA_TRUNCATED){
			helper::ReFetchTruncCol(stmt, binds, buffers);
		}

		if(fetchret != 0)
			helper::ThrowIfFetchFialed(stmt, fetchret);

		if(fetchret == MYSQL_NO_DATA)
			break;

		TinySTL::Tuple<Args...> row{};

		detail::TupleSetter(row, binds);
		tuples.emplace_back(std::move(row));	
		
		fetchret = helper::Fetch(stmt);
	}

}

} // namespace zxy

#endif // _ZXY_MYSQL_OUTPUT_BINDER_H
