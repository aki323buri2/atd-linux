//atd.string.h
#ifndef __atd_string_h__
#define __atd_string_h__
#include "atd.h"
namespace atd {;;
struct string;
struct strings;
//====================================================
//= struct atd::string
//====================================================
struct string : public object, public std::string
{
	//文字コードコンタバータ
	struct encoder;

	string();
	string(const char *that);
	string(const std::string &that);
	string(int size, char fill);
	string(iterator begin, iterator end);
	string(const_iterator begin, const_iterator end);

	static string format(const char *format, ...);

	static string enclose(const string &s, const string &left, const string &right);
	static strings explode(const string &s, const string &delimiter);

	//toint(), tointbase64()における'base'が0の場合、
	//Cの定数表記に従う。
	//つまり、0が先頭なら8進数、0xまたは0Xが先頭なら16進数、それ以外なら10進数
	static int toint(const string &s, int base = 0);
	static int64 toint64(const string &s, int base = 0);
	static double todouble(const string &s);

	string enclose(const string &left, const string &right) const;
	strings explode(const string &delimiter) const;
	int toint(int base = 0) const;
	int64 toint64(int base = 0) const;
	double todouble() const;

	//文字列検索
	bool startswith(const string &with) const;
	bool endswith(const string &with) const;
	static bool startswith(const string &s, const string &with);
	static bool endswith(const string &s, const string &with);
};
//====================================================
//= struct atd::strings
//====================================================
struct strings : public object, public std::vector<string>
{
	strings()																																													{ }
	strings(const string &s0)																																									{ entry(s0); }
	strings(const string &s0, const string &s1)																																					{ entry(s0, s1); }
	strings(const string &s0, const string &s1, const string &s2)																																{ entry(s0, s1, s2); }
	strings(const string &s0, const string &s1, const string &s2, const string &s3)																												{ entry(s0, s1, s2, s3); }
	strings(const string &s0, const string &s1, const string &s2, const string &s3, const string &s4)																							{ entry(s0, s1, s2, s3, s4); }
	strings(const string &s0, const string &s1, const string &s2, const string &s3, const string &s4, const string &s5)																			{ entry(s0, s1, s2, s3, s4, s5); }
	strings(const string &s0, const string &s1, const string &s2, const string &s3, const string &s4, const string &s5, const string &s6)														{ entry(s0, s1, s2, s3, s4, s5, s6); }
	strings(const string &s0, const string &s1, const string &s2, const string &s3, const string &s4, const string &s5, const string &s6, const string &s7)										{ entry(s0, s1, s2, s3, s4, s5, s6, s7); }
	strings(const string &s0, const string &s1, const string &s2, const string &s3, const string &s4, const string &s5, const string &s6, const string &s7, const string &s8)					{ entry(s0, s1, s2, s3, s4, s5, s6, s7, s8); }
	strings(const string &s0, const string &s1, const string &s2, const string &s3, const string &s4, const string &s5, const string &s6, const string &s7, const string &s8, const string &s9)	{ entry(s0, s1, s2, s3, s4, s5, s6, s7, s8, s9); }

	strings &entry(const string &s0)																																									{ return entryf( 1, &s0[0]); }
	strings &entry(const string &s0, const string &s1)																																					{ return entryf( 2, &s0[0], &s1[0]); }
	strings &entry(const string &s0, const string &s1, const string &s2)																																{ return entryf( 3, &s0[0], &s1[0], &s2[0]); }
	strings &entry(const string &s0, const string &s1, const string &s2, const string &s3)																												{ return entryf( 4, &s0[0], &s1[0], &s2[0], &s3[0]); }
	strings &entry(const string &s0, const string &s1, const string &s2, const string &s3, const string &s4)																							{ return entryf( 5, &s0[0], &s1[0], &s2[0], &s3[0], &s4[0]); }
	strings &entry(const string &s0, const string &s1, const string &s2, const string &s3, const string &s4, const string &s5)																			{ return entryf( 6, &s0[0], &s1[0], &s2[0], &s3[0], &s4[0], &s5[0]); }
	strings &entry(const string &s0, const string &s1, const string &s2, const string &s3, const string &s4, const string &s5, const string &s6)														{ return entryf( 7, &s0[0], &s1[0], &s2[0], &s3[0], &s4[0], &s5[0], &s6[0]); }
	strings &entry(const string &s0, const string &s1, const string &s2, const string &s3, const string &s4, const string &s5, const string &s6, const string &s7)										{ return entryf( 8, &s0[0], &s1[0], &s2[0], &s3[0], &s4[0], &s5[0], &s6[0], &s7[0]); }
	strings &entry(const string &s0, const string &s1, const string &s2, const string &s3, const string &s4, const string &s5, const string &s6, const string &s7, const string &s8)					{ return entryf( 9, &s0[0], &s1[0], &s2[0], &s3[0], &s4[0], &s5[0], &s6[0], &s7[0], &s8[0]); }
	strings &entry(const string &s0, const string &s1, const string &s2, const string &s3, const string &s4, const string &s5, const string &s6, const string &s7, const string &s8, const string &s9)	{ return entryf(10, &s0[0], &s1[0], &s2[0], &s3[0], &s4[0], &s5[0], &s6[0], &s7[0], &s8[0], &s9[0]);  }
	strings &entryf(int count, ...);

	static string implode(const strings &ss, const string &glue);
	string implode(const string &glue) const;
};
//====================================================
//= struct string::encoder
//====================================================
struct string::encoder : public object
{
	encoder(const string &encto, const string &encfrom);
	~encoder();

	string encode(const string &from) const;
	bool encode(const string &from, string &to) const;

private:
	//implイディオム
	struct impl;
	struct impl *impl;
	encoder(const encoder &that);
};
}//namespace atd
#endif//__atd_string_h__
