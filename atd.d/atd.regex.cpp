//atd.regex.cpp
#include "atd.h"
#include <regex.h>
using namespace atd;
//====================================================
//= struct atd::regex::impl
//= ** implイディオム！
//====================================================
struct regex::impl : public object
{
	regex_t preg;
	int error;
	std::vector<regmatch_t> pmatch;

	impl()
	: error(0), pmatch(0x100, regmatch_t())
	{
		::memset(&preg, 0, sizeof(preg));
	}
	~impl()
	{
		clear();
	}
	void clear()
	{
		::regfree(&preg);
	}
	//================================================
	//= 正規表現コンパイル
	//================================================
	bool compile(const string &pattern)
	{
		error = ::regcomp(&preg, pattern.c_str(), REG_EXTENDED | REG_NEWLINE);
		return !error;
	}
	//================================================
	//= 正規表現によるマッチング実行（グループ数を返す）
	//================================================
	int exec(const string &subject)
	{
		if (error) return false;//コンパイルに失敗している

		int r = ::regexec(&preg, subject.c_str(), pmatch.size(), &pmatch[0], 0);
		if (r != 0) return false;

		//マッチしたグループの数
		int nsub = preg.re_nsub + 1;

		return nsub;
	}
	//================================================
	//= 正規表現によるマッチングリスト作成
	//================================================
	bool match(const string &subject, strings &match)
	{
		match.clear();

		//正規表現マッチ実行（グループ数が返る）
		int nsub = exec(subject);
		if (!nsub) return false;

		//マッチリスト作成
		for (int i = 0; i < nsub; i++)
		{
			string s;
			regmatch_t &regmatch = pmatch[i];
			int so = regmatch.rm_so;
			int eo = regmatch.rm_eo;
			if (so >= 0 && eo >= 0)
			{
				s = subject.substr(so, (eo - so));
			}
			match.entry(s);
		}

		return true;
	}
	//================================================
	//= 正規表現による置換
	//================================================
	string replace(const string &subject, const string &replace)
	{
		//マッチング実行（グループ数が返る）
		int nsub = exec(subject); 
		if (!nsub) return subject;

		string s = subject;
		strings groups;//グループマッチリスト
		for (int i = 0; i < nsub; i++)
		{
			regmatch_t &regmatch = pmatch[i];
			int so = regmatch.rm_so;
			int eo = regmatch.rm_eo;
			if (so >= 0 && eo >= 0)
			{
				if (i == 0)
				{
					s = ""
						+ s.substr(0, so)
						+ replace 
						+ s.substr(eo)
						;
				}
				else
				{
					groups.entry(subject.substr(so, (eo - so)));
				}
			}
		}
		//グループマッチ展開
		if (groups.size())
		{
			for (strings::iterator b = groups.begin(), i = b, e = groups.end()
				; i != e; ++i)
			{
				string &group = *i;
				int index = (i - b) + 1;
				string tar = string::format("$%d", index);
				size_t find = s.find(tar);

				if (find == string::npos) continue;

				s.replace(find, tar.length(), group);

			}
		}
		return s;
	}
	static bool match(const string &pattern, const string &subject, strings &match)
	{
		regex re;
		re.compile(pattern);
		return re.match(subject, match);
	}
	static strings match(const string &pattern, const string &subject)
	{
		regex re;
		re.compile(pattern);
		return re.match(subject);
	}
	static string replace(const string &pattern, const string &subject, const string &replace)
	{
		regex re;
		re.compile(pattern);
		return re.replace(subject, replace);
	}
};
//====================================================
//= struct atd::regex
//====================================================
regex::regex()
: impl(new struct impl())
{
}
regex::~regex()
{
	delete impl;
}
bool regex::compile(const string &pattern)
{
	return impl->compile(pattern);
}
bool regex::match(const string &subject, strings &match)
{
	return impl->match(subject, match); 
}
strings regex::match(const string &subject)
{
	strings match;
	this->match(subject, match);
	return match;
}
string regex::replace(const string &subject, const string &replace)
{
	return impl->replace(subject, replace);
}
//----------------------------------------------------
//- static版
//----------------------------------------------------
bool regex::match(const string &pattern, const string &subject, strings &match)
{
	return impl::match(pattern, subject, match);
}
strings regex::match(const string &pattern, const string &subject)
{
	return impl::match(pattern, subject);
}
string regex::replace(const string &pattern, const string &subject, const string &replace)
{
	return impl::replace(pattern, subject, replace);
}
