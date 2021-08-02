#ifndef _ZXY_MYSQL_PRINT_H
#define _ZXY_MYSQL_PRINT_H

#include "tuple.h"
#include "type_def.h"
#include "typelist.h"
#include <memory>
#include <mysql/mysql.h>

namespace zxy{

#define SMART_POINTER_PRINT(wrapper) \
template<typename Ostream, typename T> \
Ostream& operator<<(Ostream& os, wrapper<T> const& p){ \
	if(! p) \
		os << "NULL"; \
	else \
		os << *p; \
	return os; \
};

SMART_POINTER_PRINT(std::unique_ptr)
SMART_POINTER_PRINT(std::shared_ptr)

// Output
using TinySTL::int_;
using TinySTL::Tuple;
using TinySTL::Make_Indexlist;
using TinySTL::mpl::Valuelist;

template<typename Ostream, typename ...Args, int_... Indices>
Ostream& PrintTuple(Ostream& os, Tuple<Args...> const& t, Valuelist<int_, Indices...>){
	os << '(';
	int dummy[] = { ((os << (Indices == 0 ? "" : ", ") << TinySTL::Get<Indices>(t)), 0)... };
	return os << ')';
}

template<typename Ostream, typename ...Args>
Ostream& operator<<(Ostream& os, Tuple<Args...> const& t){
	return PrintTuple(os, t, Make_Indexlist<sizeof...(Args)>{});
}

//template<typename Ostream>
//void PrintTuple(Ostream& os,TinySTL::Tuple<> const&,bool isFirst=true){
	//os<<(isFirst ? "()" : ")");
//}

//template<typename Ostream, typename ...Types>
//void PrintTuple(Ostream& os,::TinySTL::Tuple<Types...> const& t,bool isFirst=true){
	//os<<(isFirst ? "(" : ", ");
	//os<<t.getHead();
	//PrintTuple(os,t.getTail(),false);
//}

//template<typename Ostream, typename ...Types>
//Ostream& operator<<(Ostream&& os,TinySTL::Tuple<Types...> const& t){
	//PrintTuple(os,t);
	//return os;
//}
		 
template<typename Ostream, typename ...Args>
Ostream& operator<<(Ostream& os, TupleVector<Args...> const& tuples){
	for(int i = 0; i != tuples.size(); ++i)
		os << tuples[i] << '\n';

	return os;
}

} // namespace zxy

#endif // _ZXY_MYSQL_PRINT_H
