// test.cpp
#include "common.h"
#include "cobol.h"
#include "thread.h"


//####################################################
void test(
	string ebc
	, const strings &fdgs
	, const strings &keys
	, const strings &jsons
	, bool looksuffix
)
{
	path::fileinfo_t info = path::fileinfo(ebc);
	notify("");
	notify(ebc);
	info.demo(notify);

	int rsize = 0;
	struct fdglist : public std::vector<cobol::fdg>
	{
		cobol::fdg &add(const string &path)
		{
			push_back(cobol::fdg());
			cobol::fdg &fdg = back();
			std::ifstream ifs(path.c_str(), std::ios::in);
			fdg.loadcobol(ifs);
			return fdg;
		}
	} fdglist;
	for (strings::const_iterator i = fdgs.begin(), e = fdgs.end()
		; i != e; ++i)
	{
		const string &path = *i;
		cobol::fdg &fdg = fdglist.add(path);
		notify("");
		fdg.demo(notify);
	}
}
//####################################################
