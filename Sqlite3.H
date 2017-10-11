#pragma once
#include <sqlite3.h>
#include <string>
#include <tuple>
#include <vector>
#include <exception>
extern "C"
{
	#include <sched.h>
}
#include <memory>

#include <cstdint>

//DEBUG
#include <iostream>

namespace SQL{
	using std::string;
	using std::vector;
	using std::tuple;

	typedef std::runtime_error SQLRTerror;

	template <typename T>
		static T get_single(sqlite3_stmt* ppsm, int iCol){
			return T();
			(void)ppsm;
			(void)iCol;
		}

	template <>
		int get_single(sqlite3_stmt* ppsm, int iCol){
			return sqlite3_column_int(ppsm, iCol);
		}

	template <>
		double get_single(sqlite3_stmt* ppsm, int iCol){
			return sqlite3_column_double(ppsm, iCol);
		}

	template <>
		std::string get_single(sqlite3_stmt *ppsm, int iCol){
			const char * foo = reinterpret_cast<const char*>(sqlite3_column_text(ppsm,iCol));
			return(foo?string(foo):""s);
		}

	template <>
		std::int64_t get_single(sqlite3_stmt *ppsm, int iCol){
			return sqlite3_column_int64(ppsm, iCol);
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

		~Con()
		{
			sqlite3_close(db);
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

			void bindvals(vector<string>&& vals){
				int pos=0;
				for(auto& v : vals){
					sqlite3_bind_text(me,++pos,v.c_str(),v.size(),SQLITE_TRANSIENT);
				}
			}

			~PPSM(){
				sqlite3_finalize(me);
			}

		};

	public:

//> Within the initializer-list of a braced-init-list, the initializer-clauses, including
// any that result from pack expansions, are evaluated in the order in which they appear.
// That is, every value computation and side effect associated with a given initializer-clause
// is sequenced before every value computation and side effect associated with any
// initializer-clause that follows it in the comma-separated list of the initializer-list.

		template<typename ...ARGS>
		vector<tuple<ARGS...>> bindnquery(const string& query,
				vector<string>&& bindvals={}){
			PPSM ppsm(db, query);
			ppsm.bindvals(std::forward<decltype(bindvals)>(bindvals));
			vector<tuple<ARGS...> > answer {};

			while (sqlite3_step(ppsm.me) == SQLITE_ROW){
				int col = 0;
				#pragma GCC diagnostic ignored "-Wsequence-point"
				tuple<ARGS...> r { get_single<ARGS>(ppsm.me,(col++))... };
				answer.push_back(r);
			}
			return answer;
		}

		//for  update and such....
		void query_nothing(const string& query, vector<string>&& bindvals={}){
			PPSM ppsm(db,query);
			ppsm.bindvals(std::forward<decltype(bindvals)>(bindvals));
			auto result = sqlite3_step(ppsm.me);
			if(result ==SQLITE_BUSY){
				do {
					sched_yield();
					result = sqlite3_step(ppsm.me);;
				} while (result == SQLITE_BUSY);
			}
		}


               template<typename QType>
               QType query_one(const string & query, vector<string>&& bindvals={}){
                       PPSM ppsm(db, query);
                       ppsm.bindvals(std::forward<decltype(bindvals)>(bindvals));
                       if(sqlite3_step(ppsm.me) != SQLITE_ROW){
                               throw(std::runtime_error("Query did not yield answer:"+query));
                       }
                       return get_single<QType>(ppsm.me,0);
               }
};

}
