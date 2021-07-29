#ifndef _ZXY_MYSQL_PRINT_H
#define _ZXY_MYSQL_PRINT_H

#include "tuple.h"

namespace zxy{

template<
	typename Ostream, 
	template<typename > class C,
	typename ...Args>
Ostream& operator<<(Ostream& os, C<TinySTL::Tuple<Args...>> const& tuples){
	for(int i = 0; i != tuples.size(); ++i)
		os << tuples[i] << '\n';

	return os;
}

} // namespace zxy

#endif // _ZXY_MYSQL_PRINT_H
