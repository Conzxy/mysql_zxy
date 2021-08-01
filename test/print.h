#ifndef _ZXY_MYSQL_PRINT_H
#define _ZXY_MYSQL_PRINT_H

#include "tuple.h"
#include "type_def.h"

namespace zxy{

template<
	typename Ostream, 
	typename ...Args>
Ostream& operator<<(Ostream& os, TupleVector<Args...> const& tuples){
	for(int i = 0; i != tuples.size(); ++i)
		os << tuples[i] << '\n';

	return os;
}

} // namespace zxy

#endif // _ZXY_MYSQL_PRINT_H
