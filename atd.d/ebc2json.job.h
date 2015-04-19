// ebc2json.job.h
#ifndef __ebc2json_job_h__
#define __ebc2json_job_h__
#include "atd.h"
using namespace atd;

#include "cobol.h"

namespace ebc2json {;
struct job : public object 
{
	struct board;
	struct map;

	struct { string ebc, fdg, json; } path;
	struct { std::ifstream ebc; } ifs;
	struct { std::ofstream ebc, json; } ofs;
	cobol::fdg fdg;
	int64 todo, done;


	job(
		  const string &ebc
		, const string &fdg
		, const string &json
	);
	~job();

	void demo(const generic::notify &notify) const;
};
struct job::map : public object, public std::map<uchar, job *>
{
	std::ifstream ifs;
	int rsize;
	int64 fsize;

	map();
	~map();
	void clear();
	void init(const string &ebc, const strings &fdgs, const strings &keys, const strings &jsons);
	void read(bool looksuffix);

	void demo(const generic::notify &notify) const;
};
}//namespace ebc2json
#endif//__ebc2json_job_h__



