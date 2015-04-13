// test.job.h
#ifndef __test_job_h__
#define __test_job_h__
#include "common.h"
#include "cobol.h"
#include "thread.h"
using namespace atd;
struct job : public object
{
	struct { string ebc, fdg, json; } path;
	std::ifstream ifs;
	std::ofstream ofs;
	cobol::fdg fdg;
	int64 todo, done;
	job(
		  const string &ebc
		, const string &fdg
		, const string &json
	);
	~job();
	void ebcclear();
	void ebcopenappend();
	void fdgload();

	struct map;
};
struct job::map : public object, public std::map<uchar, job *>
{
	~map();
};
#endif//__test_job_h__



