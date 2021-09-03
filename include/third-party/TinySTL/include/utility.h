#ifndef _ZXY_TINYSTL_UTILITY_H
#define _ZXY_TINYSTL_UTILITY_H

#include "stl_move.h"
#include "type_traits.h"
#include "config.h"

namespace TinySTL{

	template<typename T1,typename T2>
	struct pair{
		T1 first;
		T2 second;

		pair(T1 const& _first,T2 const& _second)
			: first{_first}
			, second{_second}
		{ }
		
		explicit pair(T1&& _first=T1{},T2&& _second=T2{})
			: first{TinySTL::move(_first)}
			, second{TinySTL::move(_second)}
		{ }
	};

	template<typename T1, typename T2>
	inline pair<Decay_t<T1>, Decay_t<T2>> make_pair(T1&& x, T2&& y){
		return pair<Decay_t<T1>, Decay_t<T2>>(
			TinySTL::forward<T1>(x), TinySTL::forward<T2>(y)
		);
	}


	template<typename InIter>
	InIter Next_Iter(InIter iter){
		return ++iter;
	}

	template<typename InIter>
	InIter Prev_Iter(InIter iter){
		return --iter;
	}
}

#endif // _ZXY_TINYSTL_UTILITY_H
