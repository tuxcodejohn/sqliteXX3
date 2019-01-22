
#ifdef AAAA
#include "Sqlite3.H"
#else
#include "Sqlite3.hh"
#endif
#include <iostream>
#include <string>
#include <type_traits>

using std::string;


int main(int argc __attribute__((unused)), char *argv[] __attribute__ ((unused)) )
{
	SQL::Con db("test.db");

	auto a = db.bindnquery< double , string>("SELECT * from a  ;");
	a.push_back(db.bindnquery<double,string>("Select 3.33333333, 3.3;")[0]);
	std::cout << "erg :" <<std::endl;
	for (auto x:a){
		std::cout << std::get<0>(x) << " " << std::get<1>(x) <<" " << std::endl;
	}
	std::cout << std::endl <<
		"query mit binds: " << std::endl;
	for (auto row: db.bindnquery<int,double>("SELECT a,c from a where b like ?1;", {"%fira"})){
		std::cout << std::get<0>(row) << " " << std::get<1>(row) <<" " << std::endl;
	}

	std::cout << std::endl << "now updating :" << std::endl;

	db.query_nothing("UPDATE a set c ='0.6' where a like ?1;", {"6"});


	return 0;
}
