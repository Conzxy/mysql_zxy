#ifndef TINYSTL_STL_MOVE_H
#define TINYSTL_STL_MOVE_H

#include "type_traits.h"

namespace TinySTL{
   
	// need remove_reference of T to avoid reference collapse 
    template<typename T>
    constexpr auto move(T&& t) noexcept
		-> decltype(static_cast<Remove_reference_t<T>&&>(t)){
        return static_cast<Remove_reference_t<T>&&>(t);
    }

    // trick: reference collapse
	// you should use it with universal reference
	// e.g.
	// template<typename T> void g(T&& x) { return f(TinySTL::forward<T>(x)) }
    template<typename T>
    constexpr T&& forward(Remove_reference_t<T>& t) noexcept{
        return static_cast<T&&>(t);
    }//overload 1

    //prohibit reference to rvalue(end lifetime), trigger static_assert
    template<typename T>
    constexpr T&& forward(Remove_reference_t<T>&& t) noexcept{
        static_assert(!Is_lvalue_reference<T>::value,
                "template argument substituting T is an lvalue reference type");

        return static_cast<T&&>(t);
    }//overload 2
}
#endif //TINYSTL_STL_MOVE_H
