#include "print.h"
#include "mysql.h"
#include "mysql_exception.h"
#include "type_def.h"
#include <string>
#include <iostream>

using namespace zxy;
using namespace TinySTL;
using namespace std;

int main()
{
	string pwd;
	cout << "Please type password: ";
	cin >> pwd;
	try{
		Mysql mysql(NULL, "conzxy", pwd.c_str(), "zxy_mysql");
		
		string id = "10";
		string name = "Einstein";
		string dept_name = "Physics";
		assert(1 ==mysql.RunModification("insert into instructor values (?, ?, ?, NULL)", id, name, dept_name));
		
		
		try{
			TupleVector<string, string, string ,double> results;

			mysql.RunQuery(results, "select * from instructor");
		} catch(MysqlException const& e){
			cout << e.what() << endl;
			TupleVector<string, string, string, unique_ptr<double>> results;
			
			mysql.RunQuery(results, "select * from instructor");

			for(auto const& t : results){
				if(Get<3>(t) == nullptr)
					cout << Get<1>(t) << "'s salary is NULL" << endl;
				else
					cout << Get<1>(t) << "'s salary is " << Get<3>(t) << endl;
			}
			
			cout << results;
		}
		
	} catch(MysqlException const& e){
		cout << e.what() <<endl;
	}
}
