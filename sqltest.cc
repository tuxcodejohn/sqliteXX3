
#include "Sqlite3.H"
#include <iostream>
#include <string>

using std::string;

int main(int argc, char *argv[])
{


	SQL::Con db("test.db");

	auto a = db.bindnquery<string,string,string,string>("SELECT 1, 2,3,4  ;");
	
	std::cout << "erg :" <<std::endl;
	for (auto x:a){
		std::cout << std::get<0>(x) << " " << std::get<1>(x) <<" ";
		std::cout << std::get<2>(x) << " " << std::get<3>(x) <<" ";
	std::cout << std::endl;
	}
	std::cout << std::endl;

	return 0;
}
