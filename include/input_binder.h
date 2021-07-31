#ifndef _ZXY_INPUT_BINDER_H
#define _ZXY_INPUT_BINDER_H

#include <vector> 
#include "type_map.h"
#include "type_def.h"
#include "type_traits.h"
#include "typelist.h"
#include "stl_move.h"

namespace zxy{

using namespace TinySTL;
using namespace TinySTL::mpl;
using namespace TinySTL::mpl::TL;

namespace detail{

template<
	unsigned N, // index 
	unsigned End, // end after
	typename Arg, // check type
	bool is_mysql_type = IsMysqlType<Arg>::value> // check type whether mysql required type
struct BindInputsImpl;

// exit template recursion
template<
	unsigned N, 
	typename Arg>
struct BindInputsImpl<N, N, Arg, true>{
	static void Apply(MysqlBindVector&) 
	{ }
};

// specialization: no mysql require type 
template<unsigned N, unsigned End, typename Arg>
struct BindInputsImpl<N, End, Arg, false> {
	template<typename... Args>
	static void Apply(MysqlBindVector&){
		static_assert(sizeof...(Args) < 0,
				"There are No Mysql required type in given arguments");
	}
};

// specialization: string(including c-style and user-defined type)
template<unsigned N, unsigned End>
struct BindInputsImpl<N, End, char const*, true>{
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
		
		BindInputsImpl<N + 1, End, Next>::Apply(parameters, args...);
	}
};

template<unsigned N, unsigned End>
struct BindInputsImpl<N, End, char *, true>{
	template<typename ...Args>
	static void Apply(MysqlBindVector& parameters, char* & str, Args&... args){
		BindInputsImpl<N, End, char const*>::Apply(parameters, str, args...);
	}
};


template<unsigned N, unsigned End>
struct BindInputsImpl<N, End, std::string, true>  {
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

		BindInputsImpl<N + 1, End, Next>::Apply(parameters, args...);
	}
};

template<unsigned N, unsigned SZ, unsigned End>
struct BindInputsImpl<N, End, char const[SZ], true>{
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

		BindInputsImpl<N + 1, End, Next>::Apply(parameters, args...);
	}
};

template<unsigned N, unsigned SZ, unsigned End>
struct BindInputsImpl<N, End, char[SZ], true>{
	template<typename ...Args>
	static void Apply(MysqlBindVector& parameters, char(&str)[SZ], Args&... args){
		BindInputsImpl<N, End, char const[SZ]>::Apply(parameters, str, args...);
	}
};

// common cases
// integers and float pointing
template<unsigned N, unsigned End, typename Arg> 
struct BindInputsImpl<N, End, Arg, true>{
	template<typename ...Args>
	static void Apply(MysqlBindVector& parameters, Arg& arg, Args&... args){
		auto& parameter = parameters.at(N);
		parameter.buffer_type = TypeMap(TypeIdentity<Arg>{});
		parameter.buffer = &arg;
		parameter.is_null = NULL;
		parameter.is_unsigned = TinySTL::Is_unsigned<Arg>::value;

		using Next = typename Conditional_t<sizeof...(Args), 
			  FrontT<Typelist<Args...>>,
			  TypeIdentity<int>
				  >::type;

		BindInputsImpl<N + 1, End, Next>::Apply(parameters, args...);
	}
};

}// namespace detail

template<typename ...Args>
void BindInputs(MysqlBindVector& parameters, Args&... args){
	using Head = typename Conditional_t<sizeof...(Args),
		  FrontT<Typelist<Args...>>,
		  TypeIdentity<int>
			>::type;

	detail::BindInputsImpl<0, sizeof...(Args), Head>::Apply(parameters, args...);
}

} // namespace zxy

#endif // _ZXY_INPUT_BINDER_H
