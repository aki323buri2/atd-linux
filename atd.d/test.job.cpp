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
void job::map::invoke_ebcdecode()
{
	std::vector<thread *> tt;
	for (iterator i = begin(), e = end(); i != e; ++i)
	{
		job *j = i->second;
		thread *t = new thread(thread::function(&job::ebcdecode, j));
		tt.push_back(t);
	}
	for (std::vector<thread *>::iterator i = tt.begin(), e = tt.end()
		; i != e; ++i)
	{
		thread *t = *i;
		t->join();
	}
	cout << "ok?" << endl;
}

namespace {;
struct lock
{
	static mutex m;
	lock() { m.lock(); }
	~lock() { m.unlock(); }
};
mutex lock::m;
struct board 
{
	struct item;
	struct map;
};
struct board::item 
{
	board::map *owner;
	string caption;
	int64 todo, done;
	int64 denom;
	item(board::map *owner, const string &caption, int64 todo)
	: owner(owner), caption(caption), todo(todo), done(0)
	{
		lock lock;
		denom = todo / 20;//★
	}
	void update(int64 done);
};
static 
struct board::map : public object, public std::vector<item>
{
	item &entry(const string &caption, int64 todo)
	{
		push_back(item(this, caption, todo));
		return back();
	}
	void update();
} bb;
}//<<anonymous>>
void job::ebcdecode()
{
	string::ebcdic ebcdic;
	int rsize = fdg.rsize;
	int64 size = path::filesize(path.ebc);
	int64 todo = size / rsize;
	board::item &board = bb.entry(path::basename(path.json), todo);

	string line(rsize, 0);
	ifs.open(path.ebc.c_str(), std::ios::binary | std::ios::in);
	while (ifs && ifs.read(&line[0], line.size()))
	{
		board.update(done);
		done++;
	}
}
void board::item::update(int64 done)
{
	if (done % denom) return;
	owner->update();
}
void board::map::update()
{
	static mutex mt;
	size_t size = this->size();
	mt.lock();
	// cout << "\r";
	cout << "?" << size;
	cout << flush;
	mt.unlock();
}