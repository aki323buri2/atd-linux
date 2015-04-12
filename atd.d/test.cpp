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
	managed::objects gb;//garbage
	for (strings::const_iterator b = fdgs.begin(), e = fdgs.end(), i = b
		; i != e; ++i)
	{
		size_t offset = i - b;
		const string &key = keys[offset].substr(0, 1);
		const string &fdg = *i;
		const string &json = jsons[offset];
		struct { string ebc; } make;
		make.ebc = 
			path::dirname (json) + "/" + 
			path::basename(ebc) + "." + key + "." + 
			path::filename(fdg)
			;
		job *j = new job(make.ebc, fdg, json);
		gb.entry(j);
	}
}
//####################################################
job::job(const string &ebc, const string &fdg, const string &json)
: ebc(ebc), fdg(fdg), json(json)
{
}
job::~job()
{
	cout << "destroy" << endl;
}
