
all: test/test.db test/sqltest 

test/test.db:
	sqlite3  $@  < test/mk.sql 

test/sqltest: Sqlite3.H ./test/sqltest.cc
	g++ -std=c++14 test/sqltest.cc -I.  -Wall -ggdb -lsqlite3 -o $@ 

