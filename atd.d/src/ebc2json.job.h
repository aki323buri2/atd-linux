// ebc2json.job.h
#ifndef __ebc2json_job_h__
#define __ebc2json_job_h__
#include "atd.h"
using namespace atd;

#include "cobol.h"

namespace ebc2json {;
struct job : public object 
{
	struct map;
	struct board;

	struct { string ebc, fdg, json; } path;
	struct { std::ifstream ebc; } ifs;
	struct { std::ofstream ebc, json; } ofs;
	cobol::fdg fdg;
	int64 todo, done;
	struct board *board;

	job(
		  const string &ebc
		, const string &fdg
		, const string &json
	);
	~job();

	void attachboard(struct board *board);
	void notifyboard(int64 done);
	void translate();

	void demo(const generic::notify &notify) const;
};
//====================================================
//= struct ebc2json::map
//====================================================
struct job::map : public object, public std::map<uchar, job *>
{
	struct cleaner;

	std::ifstream ifs;
	int rsize;
	int64 fsize;

	//クリーナー（一時ファイルの削除、移動）
	struct cleaner *cleaner;

	map();
	~map();
	void clear();
	void init(const string &ebc, const strings &fdgs, const strings &keys, const strings &jsons);
	void read(bool looksuffix);

	void demo(const generic::notify &notify) const;
};
//====================================================
//= struct ebc2json::job::board
//====================================================
struct job::board : public object
{
	string json;
	int64 todo;
	board();
	virtual void notify(int64 done) = 0;
};
//====================================================
//= struct ebc2json::job::map::cleaner
//====================================================
struct job::map::cleaner : public object
{
	struct item;
	struct list;
	struct list *list;
	cleaner();
	~cleaner();
	void entry(const string &path, const string &move = "");
};
struct job::map::cleaner::item : public object
{
	string path;
	string move;
	bool done;
	item(const string &path, const string &move);
	~item();
	void cleanup();
};
struct job::map::cleaner::list : public std::vector<job::map::cleaner::item *>
{
	~list();
	void cleanup();
};
}//namespace ebc2json
#endif//__ebc2json_job_h__



