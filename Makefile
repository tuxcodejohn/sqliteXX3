

sqltest: Sqlite3.H sqltest.cc
	clang++ -std=c++11 sqltest.cc -Wall -ggdb -lsqlite3 -o $@ 

