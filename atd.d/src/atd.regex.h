//atd.regex.h
#ifndef __atd_regex_h__
#define __atd_regex_h__
#include "atd.h"
namespace atd {;;
//====================================================
//= struct atd::regex
//====================================================
struct regex : public object
{
	regex();
	~regex();
	bool compile(const string &pattern);
	bool match(const string &subject, strings &match);
	strings match(const string &subject);
	static bool match(const string &pattern, const string &subject, strings & match);
	static strings match(const string &pattern, const string &subject);

	string replace(const string &subject, const string &replace);
	static string replace(const string &pattern, const string &subject, const string &replace);
private:
	struct impl;
	struct impl *impl;
	regex(const string &regex);
	regex &operator = (const regex &); 
};
}//namespace atd
#endif//__atd_regex_h__