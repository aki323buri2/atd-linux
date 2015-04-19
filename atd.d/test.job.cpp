// test.job.cpp
#include "test.job.h"
using namespace test;
job::job(
	  const string &ebc
	, const string &fdg
	, const string &json
)
: todo(0)
, done(0)
, myboard(0)
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
job::board::board() : todo(0), done(0) { }
job::board::board(job &job) 
: ebc(job.path.ebc)
, json(job.path.json)
, todo(job.todo)
, done(job.done)
, stopped(false)
{
	caption = path::basename(json);
	job.myboard = this;
}
job::board &job::board::reload(job &job)
{
	todo = job.todo;
	done = job.done;
	return *this;
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
struct sync : public object, std::vector<job::board *>
{
	mutex lock;
	event signal;
	~sync()
	{
		for (iterator i = begin(); i != end(); )
		{
			delete *i;
			i = erase(i);
		}
	}
	void notify(const string &s)
	{
		static mutex m;
		struct lock lock(m);
		cout << s << endl;
	}
	bool stopped() const
	{
		for (const_iterator i = begin(), e = end()
			; i != e; ++i)
		{
			job::board *b = *i;
			if (!b->stopped) return false;
		}
		return true;
	}

	void watcher();
	void showboards();
};
static sync sc;
}//<<anonymouse>>
//----------------------------------------------------
void job::map::invoke_ebcdecode()
{
	sc.notify("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");

	for (iterator i = begin(), e = end(); i != e; ++i)
	{
		job *j = i->second;
		sc.push_back(new job::board(*j));
	}

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
	}
	
	for (std::vector<thread *>::iterator i = tt.begin(), e = tt.end()
		; i != e; ++i)
	{
		thread *t = *i;
		t->join();
	}
	t->join();
	sc.notify("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
}
void job::ebcdecode()
{
	int howmany = 3;
	for (int i = 0; i < howmany; i++)
	{
		done++;
		sc.signal.lock();
		if (myboard)
		{
			myboard->reload(*this);
		}
		sc.signal.signal();
		sc.signal.unlock();
		::sleep(1);
	}
	sc.signal.lock();
	myboard->stopped = true;
	sc.signal.signal();
	sc.signal.unlock();
}
void sync::watcher()
{
	while (true)
	{
		sc.signal.wait();
		if (stopped()) break;
		showboards();
	}
}
void sync::showboards()
{
	static mutex m;
	struct lock lock(m);

	strings ss;
	for (iterator i = begin(), e = end(); i != e; ++i)
	{
		job::board *board = *i;
		string caption = board->caption;
		int64 todo = board->todo;
		int64 done = board->done;

		string s = string::format(
			"%-20s : %lld/%lld"
			, caption.c_str()
			, done
			, todo
		);
		ss.entry(s);
	}
	sc.notify(ss.implode(" | "));
}