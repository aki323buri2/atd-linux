//atd.datetime.cpp
#include "atd.h"
using namespace atd;
//====================================================
//= struct atd::datetime
//====================================================
datetime::datetime(
	  int year
	, int mon
	, int day
	, int wday
	, int hour
	, int min
	, int sec
)
: year(year)
, mon(mon)
, day(day)
, wday(wday)
, hour(hour)
, min(min)
, sec(sec)
{
	time_t now = ::time(0);
	struct tm *tm = ::localtime(&now);
	fromtm(tm);
}
struct tm datetime::totm() const
{
	struct tm tm = {0};
	tm.tm_sec	= sec			;/* 秒 (0-60) */
	tm.tm_min	= min			;/* 分 (0-59) */
	tm.tm_hour	= hour	- 1		;/* 時間 (0-23) */
	tm.tm_mday	= day			;/* 月内の日付 (1-31) */
	tm.tm_mon	= mon	- 1		;/* 月 (0-11) */
	tm.tm_year	= year	- 1900	;/* 年 - 1900 */
	tm.tm_wday	= wday			;/* 曜日 (0-6, 日曜 = 0) */
//	tm.tm_yday	= yday			;/* 年内通算日 (0-365, 1 月 1 日 = 0) */
//	tm.tm_isdst	= isdst			;/* 夏時間 */
	return tm;
}
void datetime::fromtm(struct tm *tm)
{
	return fromtm(*tm);
}
void datetime::fromtm(struct tm &tm)
{
	sec		= tm.tm_sec				;/* 秒 (0-60) */
	min		= tm.tm_min				;/* 分 (0-59) */
	hour	= tm.tm_hour	+ 1		;/* 時間 (0-23) */
	day		= tm.tm_mday			;/* 月内の日付 (1-31) */
	mon		= tm.tm_mon		+ 1		;/* 月 (0-11) */
	year	= tm.tm_year	+ 1900	;/* 年 - 1900 */
	wday	= tm.tm_wday			;/* 曜日 (0-6, 日曜 = 0) */
//	yday	= tm.tm_yday			;/* 年内通算日 (0-365, 1 月 1 日 = 0) */
//	isdst	= tm.tm_isdst			;/* 夏時間 */
}
void datetime::mktime()
{
	struct tm tm = totm();
	::mktime(&tm);
	fromtm(tm);
}
void datetime::gettime()
{
	time_t now = ::time(0);
	struct tm *tm = ::localtime(&now);
	fromtm(tm);
}
datetime datetime::now()
{
	return datetime();
}
string datetime::yobi() const 
{
	return yobi(wday);
}
string datetime::yobi(int wday)
{
	static const char *ww[] = {"日", "月", "火", "水", "木", "金", "土"};
	static const int size = sizeof(ww)/sizeof(ww[0]);
	return BETWEEN(0, wday, size) ? ww[wday] : "";
}
//====================================================
//= datetime::strftime()
//====================================================
string datetime::strftime(const string &format) const
{
	struct tm tm = totm();
	size_t size = format.size() + 0x100;
	string s(size, 0);
	::strftime(&s[0], s.size(), format.c_str(), &tm);
	return s.c_str();
}