#include "mysql.h"
#include "mysql_exception.h"
#include "output_binder.h"
#include "type_def.h"
#include <iostream>

using namespace zxy;
using namespace std;

struct Double{
	double data;
};

REGISTER_TYPE(Double, MYSQL_TYPE_DOUBLE)

template<typename Ostream>
Ostream& operator<<(Ostream& os, Double const& d){
	return os << d.data;
}

template<>
struct zxy::detail::OutputBinder<Double, true>{
	static void Apply(MYSQL_BIND& bind, MysqlBuffer& buffer, bool& is_null){
		bind.buffer_type = TypeMap(TypeIdentity<Double>{});
		buffer.resize(sizeof(Double));
		bind.buffer = buffer.data();
		bind.is_null = &is_null;	
		bind.is_unsigned = TinySTL::Is_unsigned<Double>::value;
		cout << "user-defined";	
	}
};

int main()
{
	string password;
	cout << "Please type your password: ";
	cin >> password;
	try{
		Mysql mysql(NULL, "conzxy", password.c_str(), "zxy_mysql");
		
		TupleVector<string, string, string, Double> results;
		mysql.RunQuery(results, "SELECT * FROM instructor"); 
	} catch(MysqlException const& e){
		cout << e.what() << endl;
	}

}
