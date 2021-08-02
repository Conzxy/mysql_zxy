#include "mysql.h"
#include "mysql_stmt.h"
#include "output_binder.h"
#include "type_def.h"
#include <iostream>
#include <array>
#include <mysql/mysql_com.h>
#include "print.h"

using namespace std;
using namespace zxy;

int main(){

	std::string password;
	cout << "Please type your password: ";
	cin >> password;

	try{
		Mysql mysql(NULL , "conzxy", password.c_str(), "zxy_mysql");

		mysql.RunModification(
				"create table instructor"
				"(ID varchar(5),"
				"name varchar(20) not null,"
				"dept_name varchar(20),"
				"salary double,"
				"primary key(ID))"
				);

		array<string, 5> IDs = { "1", "2", "3", "4", "5" };
		array<string, 5> names = { "wu", "Gold", "Katz", "Califieri", "Zeng" };
		array<string, 5> dept_names = { "Finance", "Music", "Physics", "History", "Finance" };
		array<double, 5> salarys = { 65000, 80000, 70000, 80000, 90000 };
		
		PreparedStmt insert_stmt = mysql.GetPreparedStmt(
				"insert into instructor (ID, name, dept_name, salary) values"
				"(?, ?, ?, ?)"
				);
		
		for(unsigned i = 0; i < IDs.size(); ++i)
		{
			mysql.RunModification(insert_stmt, IDs[i], names[i], dept_names[i], salarys[i]);
		}

		TupleVector<string, string, string, double> tests;
		
		mysql.RunQuery(tests, "SELECT * FROM instructor");	
	
		cout << "All tuples from insturctor\n";	
		cout << tests << endl;
		
		tests.clear();
		
		int salary_query = 80000;
		mysql.RunQuery(tests, "SELECT * FROM instructor WHERE salary = ?", salary_query);
		cout << "Select tuple while salary = 80000 fron instructor\n";
		cout << tests;

	} catch(MysqlException const& e){
		cout << e.what() << endl;
	}
	
	
}
