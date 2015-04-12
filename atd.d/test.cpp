// test.cpp
#include "common.h"
#include "cobol.h"
#include "thread.h"

struct job : public object
{
	string ebc;
	string fdg;
	string json;
	std::ifstream ifs;
	std::ofstream ofs;
	job(const string &ebc, const string &fdg, const string &json);
	~job();

	struct map;
};
struct job::map : public object, public std::map<uchar, job *>
{
	~map();
};
//####################################################
void test(
	string ebc
	, const strings &fdgs
	, const strings &keys
	, const strings &jsons
	, bool looksuffix
)
{
	job::map jj;
	for (strings::const_iterator b = fdgs.begin(), e = fdgs.end(), i = b
		; i != e; ++i)
	{
		size_t offset = i - b;
		const string &key = keys[offset].substr(0, 1);
		const string &fdg = *i;
		const string &json = jsons[offset];

		//分活用EBCDICファイルパスの生成
		struct { string ebc; } make;
		make.ebc = 
			path::dirname (json) + "/" + 
			path::basename(ebc) + "." + key + "." + 
			path::filename(fdg)
			;
		
		job *j = new job(make.ebc, fdg, json);
		jj[key[0]] = j;
	}
}
//####################################################
job::job(const string &ebc, const string &fdg, const string &json)
: ebc(ebc), fdg(fdg), json(json)
{
}
job::~job()
{
}
job::map::~map()
{
	for (iterator i = begin(), e = end()
		; i != e; ++i)
	{
		delete i->second;
		erase(i);
	}
}
 