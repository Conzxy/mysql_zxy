#ifndef _ZXY_INPUT_BINDER_H
#define _ZXY_INPUT_BINDER_H

#include "type_map.h"
#include "type_def.h"
#include "third-party/TinySTL/include/type_traits.h"
#include "third-party/TinySTL/include/typelist.h"
#include "third-party/TinySTL/include/stl_move.h"
#include <string.h>
#include <utility>
namespace zxy{


using TinySTL::Conditional_t;
using TinySTL::mpl::Typelist;
using TinySTL::mpl::TL::FrontT;
using TinySTL::int_;
using TinySTL::mpl::Valuelist;
using TinySTL::mpl::TL::Make_IndexList;


namespace detail{

//template<
	//typename Arg, // check type
	//bool is_mysql_type = IsMysqlType<Arg>::value> // check type whether mysql required type
//struct InputBinder;

// exit template recursion
//template<
	//unsigned N, 
	//typename Arg>
//struct InputBinder<N, N, Arg, true>{
	//static void Apply(MysqlBindVector&) 
	//{ }
//};

// specialization: no mysql require type 
//template<typename Arg>
//struct InputBinder<Arg, false> {
	//static void Apply(MYSQL_BIND&, Arg const&) {
		//static_assert(sizeof(Arg) < 0,
				//"There are No Mysql required type in given arguments");
	//}
//};


//template<>
//struct InputBinder<MysqlNullValueType, true> {
	//static void Apply(MYSQL_BIND& parameter, MysqlNullValueType const&) {
		//static bool null = true;		
		//parameter.is_null = &null;
	//}
//};

// specialization: string(including c-style and user-defined type)
//template<>
//struct InputBinder<char const*, true>{
	//static void Apply(MYSQL_BIND& parameter, char const* str) {
		//parameter.buffer_type = TypeMap(TypeIdentity<char const*>{});
		//parameter.buffer = const_cast<char*>(str);
		//parameter.buffer_length = strlen(str);
		//parameter.length = &parameter.buffer_length;
		//parameter.is_null = NULL;
	//}
//};

//template<>
//struct InputBinder<char *, true>{
	//static void Apply(MYSQL_BIND& parameter, char* str) {
		//parameter.buffer_type = TypeMap(TypeIdentity<char const*>{});
		//parameter.buffer = str;
		//parameter.buffer_length = strlen(str);
		//parameter.length = &parameter.buffer_length;
		//parameter.is_null = NULL;
	//}
//};

//template<>
//struct InputBinder<std::string, true>  {
	//static void Apply(MYSQL_BIND& parameter, std::string const& str) {
		//parameter.buffer_type = TypeMap(TypeIdentity<std::string>{});
		////before C++17, std::string::data return char const*
		//parameter.buffer = const_cast<char*>(str.data());
		//parameter.buffer_length = str.size(); //O(1) so not just forward to specialization of char*
		//parameter.length = &parameter.buffer_length;
		//parameter.is_null = NULL;
	//}
//};

//template<uint64_t SZ>
//struct InputBinder<char const[SZ], true>{
	//static void Apply(MYSQL_BIND& parameter, char const(&str)[SZ]) {
		//parameter.buffer_type = TypeMap(TypeIdentity<std::string>{});
		//parameter.buffer = const_cast<char*>(str);
		//parameter.buffer_length = SZ; 
		//parameter.length = &parameter.buffer_length;
		//parameter.is_null = NULL;
	//}
//};

//template<uint64_t SZ>
//struct InputBinder<char[SZ], true>{
	//static void Apply(MYSQL_BIND& parameter, char(&str)[SZ]) {
		//InputBinder<char const[SZ]>::Apply(parameter, str);
	//}
//};

//template<uint64_t N> 
//struct InputBinder<SQLBlob<N>, true>{
	//static void Apply(MYSQL_BIND& parameter, SQLBlob<N> const& blob) { 
		//InputBinder<decltype(blob.data), true>::Apply(parameter, blob.data);
	//}
//};

//BINDINPUTSIMPL_BLOB_SPECIALIZATION(MediumBlob)
//BINDINPUTSIMPL_BLOB_SPECIALIZATION(LongBlob)


// common cases
// integers and float pointing
// time

inline void BindInput(MYSQL_BIND& parameter, std::nullptr_t const&)
{
	static bool null = true;
	parameter.is_null = &null;
}

inline void BindInput(MYSQL_BIND& parameter, char const* str)
{
	parameter.buffer_type = MYSQL_TYPE_STRING;
	parameter.buffer = const_cast<char*>(str);
	parameter.buffer_length = strlen(str);
	parameter.length = &parameter.buffer_length;
	parameter.is_null = NULL;
}

inline void BindInput(MYSQL_BIND& parameter, std::string const& str)
{

	parameter.buffer_type = MYSQL_TYPE_VARCHAR; //before C++17, std::string::data return char const*
	parameter.buffer = const_cast<char*>(str.data());
	parameter.buffer_length = str.size(); //O(1) so not just forward to specialization of char*
	parameter.length = &parameter.buffer_length;
	parameter.is_null = NULL;
}

template<uint64_t SZ>
inline void BindInput(MYSQL_BIND& parameter, char const(&str)[SZ])
{
	parameter.buffer_type = MYSQL_TYPE_STRING;
	parameter.buffer = const_cast<char*>(str);
	parameter.buffer_length = SZ; 
	parameter.length = &parameter.buffer_length;
	parameter.is_null = NULL;
}

template<typename Arg, typename = TinySTL::Enable_if_t<
	TinySTL::Disjunction_t<
		TinySTL::Is_integral<Arg>,
		TinySTL::Is_floating_point<Arg>>::value>>
inline void BindInput(MYSQL_BIND& parameter, Arg const& arg) {
	parameter.buffer_type = TypeMap(TypeIdentity<Arg>{});
	parameter.buffer = (void*)&arg;
	parameter.is_null = NULL;
	parameter.is_unsigned = TinySTL::Is_unsigned<Arg>::value;
}

inline void BindInput(MYSQL_BIND& parameter, MysqlTime const& arg) {
	parameter.buffer_type = MYSQL_TYPE_DATETIME;
	parameter.buffer = (void*)&arg;
	parameter.is_null = NULL;
}

//template<typename Arg>
//struct InputBinder<Arg, true>{
	//static void Apply(MYSQL_BIND& parameter, Arg const& arg) {
		//parameter.buffer_type = TypeMap(TypeIdentity<Arg>{});
		//parameter.buffer = static_cast<void*>(&arg);
		//parameter.is_null = NULL;
		//parameter.is_unsigned = TinySTL::Is_unsigned<Arg>::value;
		//// buffer_length and length don't need to set explicitly
	//}
//};

//template<typename Arg>
//void BindInput(MYSQL_BIND& parameter, Arg const& arg) {
	//using ArgType = TinySTL::Remove_cv_t<Arg>;
	//InputBinder<ArgType>::Apply(parameter, arg);
//}

template<typename ...Args, int_... Indices>
inline void BindInputs(MysqlBindVector& parameters, Tuple<Args...>& tuple_args, Valuelist<int_, Indices...>) {
	int dummy[] = { (BindInput(parameters[Indices], Get<Indices>(tuple_args)), 0)... };
}

}// namespace detail


template<typename ...Args>
inline void BindInputs(MysqlBindVector& parameters, Args const&... args){
	// use sta::ref instead
	Tuple<Args const&...> tuple_args(args...);
	detail::BindInputs(parameters, tuple_args, Make_IndexList<sizeof...(Args)>{});
}

} // namespace zxy

#endif // _ZXY_INPUT_BINDER_H
