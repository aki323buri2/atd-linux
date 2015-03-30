//atd.path.cpp
#include "atd.h"
using namespace atd;
#include <libgen.h>//for ::dirname(), ::basename()
//====================================================
//= struct atd::path
//====================================================
string path::dirname(const string &path)
{
	string s = path;
	return ::dirname(&s[0]);
}
#define __xpg_basename basename
string path::basename(const string &path)
#define basename __xpg_basename
{
	string s = path;
	if (s.endswith("/"))
	{
		s = s.substr(0, s.length()-1);
	}
	return ::basename(&s[0]);
}
