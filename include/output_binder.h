#ifndef _ZXY_MYSQL_OUTPUT_BINDER_H
#define _ZXY_MYSQL_OUTPUT_BINDER_H

#include "mysql_stmt.h"
#include "type_def.h"
#include "type_map.h"
#include <memory>
#include <assert.h>
#include "third-party/TinySTL/include/type_traits.h"
#include "third-party/TinySTL/include/typelist.h"
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
void BindOutputs(
		MysqlBindVector& binds,
		MysqlBufferVector& buffers);


template<
	typename Arg, 
	bool is_mysql_type = IsMysqlType<Arg>::value>
struct OutputBinder;

// not mysql required type
template<typename Arg>
struct OutputBinder<Arg, false>{
	static void Apply(MYSQL_BIND& , MysqlBuffer&){
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
	static void Apply(MYSQL_BIND& bind, MysqlBuffer& buffer){
		bind.buffer_type = MYSQL_TYPE_STRING;
		//set a valid buffer size 
		if(buffer.size() == 0)
			buffer.resize(30);
		bind.buffer = buffer.data();
		bind.buffer_length = buffer.size();
	}
};


// std::string
template<>
struct OutputBinder<std::string, true>{
	static void Apply(MYSQL_BIND& bind, MysqlBuffer& buffer) {
		bind.buffer_type = MYSQL_TYPE_VAR_STRING;
		if(buffer.size() == 0)
			buffer.resize(30);
		bind.buffer = buffer.data();
		bind.buffer_length = buffer.size();
	}
};

template<unsigned SZ>
struct OutputBinder<char[SZ], true>{
	static void Apply(MYSQL_BIND& bind, MysqlBuffer& buffer) {
		bind.buffer_type = MYSQL_TYPE_VAR_STRING;
		if(buffer.size() == 0)
			buffer.resize(SZ);
		bind.buffer = buffer.data();
		bind.buffer_length = buffer.size();
	}
};

// other type
template<typename Arg>
struct OutputBinder<Arg, true>{
	static void Apply(MYSQL_BIND& bind, MysqlBuffer& buffer){
		bind.buffer_type = TypeMap(TypeIdentity<Arg>{});
		buffer.resize(sizeof(Arg));
		bind.buffer = buffer.data();
		bind.is_unsigned = TinySTL::Is_unsigned<Arg>::value;
	}
};

// prohibit raw pointer
// represent value while may be 'null'
// should use smart pointer(resource safe and no memery leak since RAII technique)
template<typename Arg>
struct OutputBinder<Arg*, false>{
	static void Apply(MYSQL_BIND&, MysqlBuffer&){
		static_assert(sizeof(Arg) < 0,
				"You should set smart pointer(e.g. std::unique_ptr, std::shared_ptr)"
				"because raw pointer may be wild pointer");
	}
};

//smart pointer
#define WRAPPER_OF_OUTPUT_BINDER_FULL_SPECIALIZATION(type)  \
template<typename Arg> \
struct OutputBinder<type<Arg>, false>{ \
	static void Apply(MYSQL_BIND& bind, MysqlBuffer& buffer){ \
		OutputBinder<Arg>::Apply(bind, buffer); \
	} \
};

WRAPPER_OF_OUTPUT_BINDER_FULL_SPECIALIZATION(std::unique_ptr)
WRAPPER_OF_OUTPUT_BINDER_FULL_SPECIALIZATION(std::shared_ptr)

using TinySTL::mpl::Typelist; //Typelist
using TinySTL::mpl::Valuelist;
using TinySTL::mpl::TL::Make_IndexList;
using TinySTL::mpl::TL::Type_At;
using TinySTL::mpl::int_;
		
template<typename TL, int_... Indices>
void BindOutputsImpl(
		MysqlBindVector& binds,
		MysqlBufferVector& buffers,
		Valuelist<int_, Indices...>)
{ 
	int dummy[] = { ((OutputBinder<Type_At<TL, Indices>>::Apply(
					binds[Indices], 
					buffers[Indices])), 0)... };
}

template<typename ...Args>
void BindOutputs(
		MysqlBindVector& binds,
		MysqlBufferVector& buffers)
{
	assert(binds.size() == buffers.size());
	
	BindOutputsImpl<Typelist<Args...>>(
			binds, buffers, 
			Make_IndexList<sizeof...(Args)>{});
}

///////////////////////////////
// Result Setter
//////////////////////////////

char const kNullErrorMessage[] = "null value is only allowed to present by smart pointer";

// no need to IsMysqlType
// because call TupleSetter after BindOutputs()
template<typename ...Args>
void TupleSetter(TinySTL::Tuple<Args...>& tuple, MysqlBindVector& binds);

template<typename Arg>
void TupleSetter(Arg& arg, MYSQL_BIND& bind){
	if(*bind.is_null)
		throw MysqlException{kNullErrorMessage};

	arg = *reinterpret_cast<Arg*>(bind.buffer);
}

#define WRAPPER_OF_TUPLESETTER_FULL_SPECIALIZATION(type) \
template<typename T> \
void TupleSetter(type<T>& arg, MYSQL_BIND& bind){ \
	if(*bind.is_null) \
		arg.reset(); \
	else \
		arg.reset(new T(*reinterpret_cast<T*>(bind.buffer))); \
}

WRAPPER_OF_TUPLESETTER_FULL_SPECIALIZATION(std::unique_ptr)
WRAPPER_OF_TUPLESETTER_FULL_SPECIALIZATION(std::shared_ptr)

template<>
inline void TupleSetter<std::string>(std::string& str, MYSQL_BIND& bind){
	if(*bind.is_null)
		throw MysqlException{kNullErrorMessage};
	
	str = reinterpret_cast<char*>(bind.buffer);
}

template<typename ...Args, int_ ...Indices>
void SetTupleImpl(
		TinySTL::Tuple<Args...>& tuple, 
		MysqlBindVector& binds,
		Valuelist<int_, Indices...>)
{
	int dummy[] = { (TupleSetter(Get<Indices>(tuple), binds[Indices]), 0)... };
}

template<typename ...Args>
void SetTuple(
		TinySTL::Tuple<Args...>& tuple, 
		MysqlBindVector& binds)
{
	SetTupleImpl(tuple, binds, Make_IndexList<sizeof...(Args)>{});
}

} // namespace detail


template<typename ...Args>
void SetResultset(TupleVector<Args...>& tuples, PreparedStmt const& stmt){
	helper::ThrowIfNotRequiredFieldCount(stmt, sizeof...(Args));	
			
	MysqlBindVector binds(stmt.field_count());
	MysqlBufferVector buffers(stmt.field_count());
	MysqlIsNullVector is_nulls(stmt.field_count());
	MysqlBindLengthVector lengths(stmt.field_count());	

	detail::BindOutputs<Args...>(binds, buffers);
	
	for(unsigned i = 0; i != stmt.field_count(); ++i)
	{
		binds[i].length = &lengths[i];
		binds[i].is_null = &is_nulls[i];
	}

	helper::BindResultAndExec(stmt, binds);
	
	auto fetchret = stmt.Fetch();

	while(true){
		if(fetchret == MYSQL_NO_DATA)
			break;

		if(fetchret == 1)
			throw MysqlException{ stmt.stmt() };
		
		if(fetchret == MYSQL_DATA_TRUNCATED){
			helper::ReFetchTruncCol(stmt, binds, buffers, lengths);
		}

		Tuple<Args...> row{};

		detail::SetTuple(row, binds);
		tuples.emplace_back(std::move(row));	
		
		fetchret = stmt.Fetch();
	}

}

} // namespace zxy

#endif // _ZXY_MYSQL_OUTPUT_BINDER_H
