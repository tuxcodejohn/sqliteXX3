
#include "Sqlite3.H"
#include <iostream>
#include <string>

using std::string;

int main(int argc, char *argv[])
{


	SQL::Con db("test.db");

	auto a = db.bindnquery< double , string>("SELECT * from a  ;");
	a.push_back(db.bindnquery<double,string>("Select 3.33333333, 3.3;")[0]);
	std::cout << "erg :" <<std::endl;
	for (auto x:a){
		std::cout << std::get<0>(x) << " " << std::get<1>(x) <<" ";
	std::cout << std::endl;
	}
	std::cout << std::endl;

	return 0;
}
