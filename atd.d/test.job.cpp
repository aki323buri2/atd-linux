// test.job.cpp
#include "test.job.h"
job::job(
	  const string &ebc
	, const string &fdg
	, const string &json
)
: todo(0)
, done(0)
{
	path.ebc = ebc;
	path.fdg = fdg;
	path.json = json;
}
job::~job()
{
}
void job::ebcclear()
{
	ofs.open(path.ebc.c_str(), std::ios::binary | std::ios::out);
	ofs.close();
}
void job::ebcopenappend()
{
	ofs.open(path.ebc.c_str(), std::ios::binary | std::ios::app);
}
void job::fdgload()
{
	std::ifstream ifs(path.fdg.c_str(), std::ios::in);
	fdg.loadcobol(ifs);
}
job::map::~map()
{
	//メモリの解放
	for (iterator i = begin(), e = end()
		; i != e; ++i)
	{
		delete i->second;
		erase(i);
	}
}
//----------------------------------------------------
namespace {;//<<anonymouse>>
struct lock : public object
{
	struct mutex &mutex;
	lock(struct mutex &mutex) : mutex(mutex) { mutex.lock(); }
	~lock() { mutex.unlock(); }
};
struct automutex : public mutex
{
	automutex() { lock(); }
	~automutex() { unlock(); }
};
struct event : public cond
{
	struct mutex mutex;
	void lock()
	{
		mutex.lock();
	}
	void unlock()
	{
		mutex.unlock();
	}
	void wait()
	{
		cond::wait(mutex);
	}
};
static 
struct sync : public object
{
	mutex lock;
	event signal;
	void notify(const string &s)
	{
		struct lock lock(this->lock);
		cout << s << endl;
	}
	void watcher();
} sc;
}//<<anonymouse>>
//----------------------------------------------------
void job::map::invoke_ebcdecode()
{
	sc.notify("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");

	managed::objects oo;
	std::vector<thread *> tt;

	thread *t = new thread(&sync::watcher, &sc);
	tt.push_back(t);
	oo.entry(t);

	for (iterator i = begin(), e = end(); i != e; ++i)
	{
		job *j = i->second;
		thread *t = new thread(&job::ebcdecode, j);
		tt.push_back(t);
		oo.entry(t);
		sc.notify(j->path.ebc);
	}
	
	for (std::vector<thread *>::iterator i = tt.begin(), e = tt.end()
		; i != e; ++i)
	{
		thread *t = *i;
		t->join();
	}
	sc.notify("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
}
void job::ebcdecode()
{
	int howmany = 3;
	for (int i = 0; i < howmany; i++)
	{
		sc.signal.signal();
		::sleep(2);
	}
	sc.notify(path.json);
}
void sync::watcher()
{
	while (true)
	{
		sc.signal.wait();
		sc.notify("signal!!");
	}
}