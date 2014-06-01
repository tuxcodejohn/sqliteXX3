

sqltest: Sqlite3.H sqltest.cc
	g++-4.9 -std=c++11 sqltest.cc -Wall -ggdb -lsqlite3 -o $@ 

