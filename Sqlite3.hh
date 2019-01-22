#pragma once
#include <sqlite3.h>
#include <string>
#include <tuple>
#include <vector>
#include <exception>
#include <cstdint>
//#include <experimental/optional>

namespace SQL{
	using std::string;
	using std::vector;
	using std::tuple;
	//using std::experimental::optional;

	typedef std::runtime_error SQLRTerror;

	template <typename T>
		T get_single(sqlite3_stmt* ppsm, int iCol);

	template <> inline
		int get_single(sqlite3_stmt* ppsm, int iCol){
			return sqlite3_column_int(ppsm, iCol);
		}

	template <> inline
		double get_single(sqlite3_stmt* ppsm, int iCol){
			return sqlite3_column_double(ppsm, iCol);
		}

	template <> inline
		std::string get_single(sqlite3_stmt *ppsm, int iCol){
			const char * foo = reinterpret_cast<const char*>(sqlite3_column_text(ppsm,iCol));
			return(foo?string(foo):"");
		}

	template <> inline
		std::int64_t get_single(sqlite3_stmt *ppsm, int iCol){
			return sqlite3_column_int64(ppsm, iCol);
		}

         // template <>
	 // optional<std::string> get_single(sqlite3_stmt *ppsm, int iCol){
	 // 	const char * foo = reinterpret_cast<const char*>(
	 // 	sqlite3_column_text(ppsm,iCol));
	 // 	return( foo ? optional<std::string>(foo) :
	 // 		optional<std::string>());
	 // }


	template<size_t ... I, typename ...ARGS>
	inline std::tuple<ARGS...>  get_row_(
		sqlite3_stmt* ppsm,
		std::index_sequence<I...> ,
		tuple<ARGS...>)
	{
		return std::make_tuple(get_single<ARGS>(ppsm, I)...);
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

	struct PPSM
	{
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

		inline void bindvals(vector<string>&& vals){
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

	template<typename ...ARGS>
	vector<tuple<ARGS...>> bindnquery(const string& query,
			vector<string>&& bindvals={}){
		PPSM ppsm(db, query);
		ppsm.bindvals(std::forward<decltype(bindvals)>(bindvals));
		vector<tuple<ARGS...> > answer {};

		using Result = tuple<ARGS ...>;

		while (sqlite3_step(ppsm.me) == SQLITE_ROW){
			answer.push_back(
				get_row_(
					ppsm.me,
					std::make_index_sequence<sizeof...(ARGS)>{},
					Result{}));
		}
		return answer;
	}

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
