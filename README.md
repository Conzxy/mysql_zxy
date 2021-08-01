# mysql_zxy

This is a simple and convenient mysql interface written in C++11.
It supports type deduction and it's type safe.

## type deduction
Modification support variadic input parameters:
```cpp
PreparedStmt insert_stmt = mysql.GetPreparedStmt(
		"insert into instructor (ID, name, dept_name, salary) values"
		"(?, ?, ?, ?)"
		);

mysql.RunModification(insert_stmt, IDs[i], names[i], dept_names[i], salarys[i]);
```
Query also support variadic output parameters and input paramters:
```cpp
TupleVector<string, string, string, double> tests;

mysql.RunQuery(tests, "SELECT * FROM instructor");	
cout << "All tuples from insturctor\n";	
cout << tests << endl;

int salary_query = 80000;
mysql.RunQuery(tests, "SELECT * FROM instructor WHERE salary = ?", salary_query);
cout << "Select tuple while salary = 80000 fron instructor\n";
cout << tests;
```

# type safe
The Mysql interface is implemented by using C++ template technique, unregistered types will trigger assertion
