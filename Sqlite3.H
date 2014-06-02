
#include <sqlite3.h>
#include <string>
#include <tuple>
#include <vector>
#include <exception>
#include <memory>

//DEBUG
#include <iostream>

namespace SQL{
	using std::string;
	using std::vector;
	using std::tuple;

	typedef std::runtime_error SQLRTerror;


	template <typename T>
		T get_single(sqlite3 *db, sqlite3_stmt* ppsm, int iCol){
			return T();
		}

	template <>
		int get_single(sqlite3 *db,sqlite3_stmt* ppsm, int iCol){
			return sqlite3_column_int(ppsm, iCol);
		}
	
	template <>
		double get_single(sqlite3 *db,sqlite3_stmt* ppsm, int iCol){
			return sqlite3_column_double(ppsm, iCol);
		}

	template <>
		std::string get_single(sqlite3 *db, sqlite3_stmt *ppsm, int iCol){
			const char * foo = reinterpret_cast<const char*>(sqlite3_column_text(ppsm,iCol));
			return(string(foo));
		}


class Con{
	protected:
		sqlite3 *db;

	private:
		const string dbfilename;

	public:
		Con(const string &dbfile):
			dbfilename(dbfile)
	{
		int erg;
		erg = sqlite3_open(dbfilename.c_str(),&db);
		if(erg != SQLITE_OK){
			throw(SQLRTerror(sqlite3_errmsg(db)));
		}
	}


	private:
		struct PPSM{
			sqlite3_stmt * me;

			PPSM(sqlite3 *db , const string &query):
				me(nullptr)
			{
				if (sqlite3_prepare_v2( db, query.c_str(), query.size(),
							&me, nullptr)
						!= SQLITE_OK){
					throw(SQLRTerror(sqlite3_errmsg(db)));
				}
			}


			~PPSM(){
				sqlite3_finalize(me);
			}

		};



	public:

		template<typename ...ARGS>
		vector<tuple<ARGS...> > bindnquery(const string& query,
				vector<string> bindvals={}){

			PPSM ppsm(db, query);
			//binds....

			vector<tuple<ARGS...> > answer {};

			while (sqlite3_step(ppsm.me) == SQLITE_ROW){
				int col = 0;
				tuple<ARGS...> r { get_single<ARGS>(db,ppsm.me,col++)... };
				answer.push_back(r);
			}

			return answer;

		}

};


}
