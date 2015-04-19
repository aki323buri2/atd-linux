// test.job.h
#ifndef __test_job_h__
#define __test_job_h__
#include "common.h"
#include "cobol.h"
#include "thread.h"
using namespace atd;
namespace test {;
struct job : public object
{
	struct board;
	struct map;

	struct { string ebc, fdg, json; } path;
	std::ifstream ifs;
	std::ofstream ofs;
	cobol::fdg fdg;
	int64 todo, done;
	board *myboard;
	
	job(
		  const string &ebc
		, const string &fdg
		, const string &json
	);
	~job();
	void ebcclear();
	void ebcopenappend();
	void fdgload();

	//EBCDICデコード
	void ebcdecode();

};
struct job::board : public object
{
	string ebc;
	string json;
	string caption;
	int64 todo;
	int64 done;
	bool stopped;
	board();
	board(job &job);
	board &reload(job &job);
};
struct job::map : public object, public std::map<uchar, job *>
{
	~map();
	void invoke_ebcdecode();
	void join();
};
}//namespace test {;
#endif//__test_job_h__



