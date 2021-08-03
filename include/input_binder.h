#ifndef _ZXY_INPUT_BINDER_H
#define _ZXY_INPUT_BINDER_H

#include "type_map.h"
#include "type_def.h"
#include "type_traits.h"
#include "typelist.h"
#include "stl_move.h"
#include <string.h>

namespace zxy{

using TinySTL::Conditional_t;
using TinySTL::mpl::Typelist;
using TinySTL::mpl::TL::FrontT;

template<typename ...Args>
void BindInputs(MysqlBindVector& parameters, Args&... args);

namespace detail{

template<
	unsigned N, // index 
	unsigned End, // end after
	typename Arg, // check type
	bool is_mysql_type = IsMysqlType<Arg>::value> // check type whether mysql required type
struct InputBinder;

// exit template recursion
template<
	unsigned N, 
	typename Arg>
struct InputBinder<N, N, Arg, true>{
	static void Apply(MysqlBindVector&) 
	{ }
};

// specialization: no mysql require type 
template<unsigned N, unsigned End, typename Arg>
struct InputBinder<N, End, Arg, false> {
	template<typename... Args>
	static void Apply(MysqlBindVector&){
		static_assert(sizeof...(Args) < 0,
				"There are No Mysql required type in given arguments");
	}
};

// specialization: string(including c-style and user-defined type)
template<unsigned N, unsigned End>
struct InputBinder<N, End, char const*, true>{
	template<typename ...Args>
	static void Apply(MysqlBindVector& parameters, char const* & str, Args&... args){
		auto& parameter = parameters.at(N);
		parameter.buffer_type = TypeMap(TypeIdentity<char const*>{});
		parameter.buffer = const_cast<char*>(str);
		parameter.buffer_length = strlen(str);
		parameter.length = &parameter.buffer_length;
		parameter.is_null = NULL;

		using Next = typename Conditional_t<sizeof...(Args), 
			  FrontT<Typelist<Args...>>,
			  TypeIdentity<int>  
				  >::type;
		
		InputBinder<N + 1, End, Next>::Apply(parameters, args...);
	}
};

template<unsigned N, unsigned End>
struct InputBinder<N, End, char *, true>{
	template<typename ...Args>
	static void Apply(MysqlBindVector& parameters, char* & str, Args&... args){
		InputBinder<N, End, char const*>::Apply(parameters, str, args...);
	}
};


template<unsigned N, unsigned End>
struct InputBinder<N, End, std::string, true>  {
	template<typename ...Args>
	static void Apply(MysqlBindVector& parameters, std::string& str, Args&... args){
		auto& parameter = parameters.at(N);
		parameter.buffer_type = TypeMap(TypeIdentity<std::string>{});
		//before C++17, std::string::data return char const*
		parameter.buffer = const_cast<char*>(str.data());
		parameter.buffer_length = str.size(); //O(1) so not just forward to specialization of char*
		parameter.length = &parameter.buffer_length;
		parameter.is_null = NULL;

		using Next = typename Conditional_t<sizeof...(Args), 
			  FrontT<Typelist<Args...>>,
			  TypeIdentity<int> 
				  >::type;

		InputBinder<N + 1, End, Next>::Apply(parameters, args...);
	}
};

template<unsigned N, unsigned SZ, unsigned End>
struct InputBinder<N, End, char const[SZ], true>{
	template<typename ...Args>
	static void Apply(MysqlBindVector& parameters, char const(&str)[SZ], Args& ...args){
		auto& parameter = parameters.at(N);
		parameter.buffer_type = TypeMap(TypeIdentity<std::string>{});
		parameter.buffer = const_cast<char*>(str);
		parameter.buffer_length = SZ; 
		parameter.length = &parameter.buffer_length;
		parameter.is_null = NULL;

		using Next = typename Conditional_t<sizeof...(Args), 
			  FrontT<Typelist<Args...>>,
			  TypeIdentity<int> 
				  >::type;

		InputBinder<N + 1, End, Next>::Apply(parameters, args...);
	}
};

template<unsigned N, unsigned SZ, unsigned End>
struct InputBinder<N, End, char[SZ], true>{
	template<typename ...Args>
	static void Apply(MysqlBindVector& parameters, char(&str)[SZ], Args&... args){
		InputBinder<N, End, char const[SZ]>::Apply(parameters, str, args...);
	}
};

#define BINDINPUTSIMPL_BLOB_SPECIALIZATION(type) \
template<unsigned N, unsigned End>\
struct InputBinder<N, End, type, true>{\
	template<typename ...Args>\
	static void Apply(MysqlBindVector& parameters, type& blob, Args& ...args){\
		InputBinder<N, End, decltype(blob.data)>::Apply(parameters, blob, args...);\
	}\
};

BINDINPUTSIMPL_BLOB_SPECIALIZATION(TinyBlob)
BINDINPUTSIMPL_BLOB_SPECIALIZATION(Blob)
BINDINPUTSIMPL_BLOB_SPECIALIZATION(MediumBlob)
BINDINPUTSIMPL_BLOB_SPECIALIZATION(LongBlob)

// common cases
// integers and float pointing
// time
template<unsigned N, unsigned End, typename Arg> 
struct InputBinder<N, End, Arg, true>{
	template<typename ...Args>
	static void Apply(MysqlBindVector& parameters, Arg& arg, Args&... args){
		auto& parameter = parameters.at(N);
		parameter.buffer_type = TypeMap(TypeIdentity<Arg>{});
		parameter.buffer = static_cast<void*>(&arg);
		parameter.is_null = NULL;
		parameter.is_unsigned = TinySTL::Is_unsigned<Arg>::value;
		// buffer_length and length don't need to set explicitly
		
		using Next = typename Conditional_t<sizeof...(Args), 
			  FrontT<Typelist<Args...>>,
			  TypeIdentity<int>
				  >::type;

		InputBinder<N + 1, End, Next>::Apply(parameters, args...);
	}
};

}// namespace detail

template<typename ...Args>
void BindInputs(MysqlBindVector& parameters, Args&... args){
	using Head = typename Conditional_t<sizeof...(Args),
		  FrontT<Typelist<Args...>>,
		  TypeIdentity<int>
			>::type;

	detail::InputBinder<0, sizeof...(Args), Head>::Apply(parameters, args...);
}

} // namespace zxy

#endif // _ZXY_INPUT_BINDER_H
