#include "mysql.h"
#include "mysql_exception.h"
#include <iostream>

using namespace zxy;
using namespace std;

int main(int argc, char* argv[])
{
	if(argc != 2)
		cout << "Usage : ./" << argv[0] << "<password>" << endl;
	
	try{
		Mysql mysql(NULL, "conzxy", argv[1], "zxy_mysql");

		TupleVector<string, string, string ,double> results;
		mysql.RunQuery(results, "select * from instructor where id = ?", "1; drop table instructor");
		assert(results.size() == 0);	


	} catch(MysqlException const& e){
		cout << e.what() << endl;
	}
	

}
