//
// Created by 谁敢反对宁宁，我就打爆他狗头！ on 2021/1/5.
//

#ifndef TINYSTL_CONFIG_H
#define TINYSTL_CONFIG_H

#include <cstdlib>

#define STL_ TinySTL::
#define STD_ std::
#define TL_ mpl::TL::
#define STL_ENABLE_IF(condition, type) TinySTL::Enable_if_t<(condition), type> = 0
using sz_t=std::size_t;

// In C++11, constexpr function is incomplete
// 1): constexpr used in non-static member function impiles
//  const member function
// 2): constexpr function don't allowed to declare local variable and for/while/if
#if __cplusplus >= 201402L
#define TINYSTL_CONSTEXPR constexpr
#else
#define TINYSTL_CONSTEXPR inline 
#endif

#endif //TINYSTL_CONFIG_H
