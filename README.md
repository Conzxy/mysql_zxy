# mysql_zxy
This is a simple and convenient mysql interface written in C++11.

It supports **type deduction** and it's **type safe**.

## init
Use the Mysql class to build connection to mysql server
```cpp
Mysql mysql(host, user, password, db)
//port default is 3306
```
Resource holding class use RAII technique, so user no need explicitly to destroy them.

## type deduction
### Modification
Supports bind variadic input parameters:
```cpp
// You should get prepared statement throught GetPreparedStmt()
PreparedStmt insert_stmt = mysql.GetPreparedStmt(
		"insert into instructor (ID, name, dept_name, salary) values"
		"(?, ?, ?, ?)"
		);

for(int i = 0; i < SIZE; ++i){
	mysql.RunModification(insert_stmt, IDs[i], names[i], dept_names[i], salarys[i]);
}
```
### Query
Also supports input paramters:
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
### handle null value
You should use **smart pointer**(such as *std::unique_ptr*, *std::shared_ptr*) to present null value:
```cpp
TupleVector<string, string, string, unique_ptr<double>> results;

mysql.RunQuery(results, "select * from instructor");

for(auto const& t : results){
	if(Get<3>(t) == nullptr)
		cout << Get<1>(t) << "'s salary is NULL" << endl;
	else
		cout << Get<1>(t) << "'s salary is " << Get<3>(t) << endl;
}
```

## type safe
The Mysql interface is implemented by using C++ template technique, unregistered types will trigger assertion
```cpp
struct Double{
	double data;
};

TupleVector<string, string, string, Double> result;
mysql.RunQuery(result, "SELECT * FROM insturctor");
// trigger static asserttion
```
Also ,if you want to support self-defined type, you can register and specialize input and output binder, but I don't suggest do it.

## injection safe
Mysql provide prepared statement API, so the interface also wrap it to PreparedStmt RAII class.

It ensure SQL injection safe:

```cpp
TupleVector<string, string, string ,double> results;
mysql.RunQuery(results, "select * from instructor where id = ?", "1; drop table instructor");
assert(results.size() == 0);	
```

