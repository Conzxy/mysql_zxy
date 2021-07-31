//
// Created by 谁敢反对宁宁，我就打爆他狗头！ on 2021/1/5.
//

#ifndef TINYSTL_CONFIG_H
#define TINYSTL_CONFIG_H

#include <cstdlib>

#define STL_ TinySTL::
#define STD_ std::
#define TL_ mpl::TL::
#define STL_MOVE(val) TinySTL::move(val)
#define STL_SWAP(val1,val2) TinySTL::swap(val1,val2)
#define STL_FORWARD(type, args) TinySTL::forward<type>(args)
#define STL_ENABLE_IF(condition, type) TinySTL::Enable_if_t<(condition), type> = 0
using sz_t=std::size_t;

#endif //TINYSTL_CONFIG_H
