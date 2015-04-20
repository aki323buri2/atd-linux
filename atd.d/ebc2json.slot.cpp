// ebc2json.slot.cpp
#include "ebc2json.slot.h"
using namespace ebc2json;
slot::list::list(const job::map &map)
{
	for (job::map::const_iterator i = map.begin(), e = map.end()
		; i != e; ++i)
	{
		struct job *job = i->second;
		push_back(new slot(&job::translate, job));
	}
}
slot::list::~list()
{
	clear();
}
void slot::list::clear()
{
	for (iterator i = begin(); i != end(); )
	{
		delete *i;
		i = erase(i);
	}
}
slot::slot(void (job::*method)(), struct job *job)
: thread(method, job)
, job(job)
, board(new struct board())
{
	job->attachboard(board);
}
slot::~slot()
{
	//boardはjob側で破棄するのでdeleteしない！
}
void slot::list::start()
{
	for (iterator i = begin(), e = end(); i != e; ++i)
	{
		(*i)->start();
	}
}
void slot::list::join()
{
	for (iterator i = begin(), e = end(); i != e; ++i)
	{
		(*i)->join();
	}
}
void slot::board::notify(int64 done)
{
	cout << json << " " << done << "/" << todo << endl;
}