// ebc2json.slot.h
#ifndef __ebc2json_slot_h__
#define __ebc2json_slot_h__
#include "atd.h"
using namespace atd;
#include "ebc2json.job.h"
#include "thread.h"
namespace ebc2json {;
struct slot : public thread 
{
	struct list;
	struct board;

	struct job *job;
	struct board *board;
	slot(void (job::*method)(), struct job *job);
	~slot();
};
struct slot::list : public object, public std::vector<slot *>
{
	list(const job::map &map);
	~list();
	void clear();
	void start();
	void join();
};
struct slot::board : public job::board
{
	void notify(int64 done);
};
}//namespace ebc2json
#endif//__ebc2json_slot_h__