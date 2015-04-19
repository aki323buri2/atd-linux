// ebc2json.job.cpp
#include "common.h"
#include "ebc2json.job.h"
using namespace ebc2json;
job::job(
	  const string &ebc
	, const string &fdg
	, const string &json
)
: todo(0), done(0)
{
	path.ebc = ebc;
	path.fdg = fdg;
	path.json = json;
	this->fdg.loadcobol(path.fdg);
}
job::~job()
{
}
job::map::map()
{
}
job::map::~map()
{
	clear();
}
void job::map::clear()
{
	for (iterator i = begin(); i != end(); ++i)
	{
		delete i->second;
		iterator p = i;
		erase(p);
	}
}
void job::map::init(const string &ebc, const strings &fdgs, const strings &keys, const strings &jsons)
{
	for (strings::const_iterator 
		  f = fdgs .begin(), fe = fdgs .end()
		, k = keys .begin(), ke = keys .end()
		, j = jsons.begin(), je = jsons.end()
		; f != fe && k != ke && j != je; ++f, ++k, ++j
	)
	{
		const string &key = *k;
		const string &fdg = *f;
		const string &json = *j;
		struct { string ebc; } split;
		split.ebc = ebc + "." + key + "." + path::filename(fdg) + ".tran.json";

		insert(value_type(key[0], new job(split.ebc, fdg, json)));
	}
}
void job::map::read(const string &ebc, bool looksuffix)
{
}
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void job::map::demo(const generic::notify &notify) const 
{
	for (const_iterator i = begin(), e = end(); i != e; ++i)
	{
		i->second->demo(notify);
	}
}
void job::demo(const generic::notify &notify) const 
{
	generic::notifyf notifyf = notify;
	notifyf(
		"*> %s %lld/%lld"
		, path.json.c_str(), todo, done
	);
}