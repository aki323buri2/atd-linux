//atd.datetime.h
#ifndef __atd_datetime_h__
#define __atd_datetime_h__
#include "atd.h"
namespace atd {;;
//====================================================
//= struct atd::datetime
//====================================================
struct datetime : public object
{
	int year;
	int mon;
	int day;
	int wday;
	int hour;
	int min;
	int sec;

	datetime();
	datetime(
 		  int year
		, int mon
		, int day
//		, int wday
		, int hour
		, int min
		, int sec
	);

	struct tm totm() const;
	void fromtm(struct tm *tm);
	void fromtm(struct tm &tm);
	void mktime();
	void gettime();
	static datetime now();

	string yobi() const;
	static string yobi(int wday);

	//ゼロ
	static datetime zero();

	//比較
	bool eq(const datetime &that) const;
	bool lt(const datetime &that) const;
	bool gt(const datetime &that) const;
	bool operator == (const datetime &that) const;
	bool operator != (const datetime &that) const;
	bool operator <  (const datetime &that) const;
	bool operator <= (const datetime &that) const;
	bool operator >  (const datetime &that) const;
	bool operator >= (const datetime &that) const;

	string strftime(const string &format = "%Y-%m-%d %H:%M:%S") const;
	/*
	書式	置き換わる文字列
	%a	ロケールの簡略化された曜日の名前
	%A	ロケールの簡略化されていない曜日の名前
	%b	ロケールの簡略化された月の名前
	%B	ロケールの簡略化されていない月の名前
	%c	ロケールの適切な日付と時刻 (dd hh:mm:ss)
	%C	西暦の年を 100 で除算し整数で切り捨てた 10 進数 (00 ～ 99)
	%d	10 進数で月の日 (01～31)
	%D	"%m/%d/%y"と等価な日付表現
	%e	10 進数で月の日 ( 1～31) 1 桁の場合は空白文字を前につける
	%F	"%Y-%m-%d" と等価な日付表現
	%g	暦週に基づく年の下 2 桁を表す 10 進数 (00 ～ 99)
	%G	暦週に基づく年を表す 10 進数 (例.2007)
	%h	"%b" と等価な表現
	%H	24 時間制の時間を表す 10 進数 (00 ～ 23)
	%I	12 時間制の時間を表す 10 進数 (01 ～ 12)
	%j	年間通した日(001 ～ 366）
	%m	月(01 ～ 12)
	%M	分(00 ～ 59)
	%n	改行文字
	%p	ロケールの 12 時間制に関連した午前/午後の表記 (AM / PM)
	%r	ロケールの 12 時間制での時刻表現
	%S	秒 (00 ～ 60)
	%t	水平タブ文字
	%T	"%H:%M:%S" に等価な時刻表現
	%u	暦週の中の日の序数 (1 ～ 7) 月曜日は 1
	%U	年間の経過した週の数 (最初の日曜日を 1 日目としてカウント)
	%V	暦週の序数 (01 ～ 53)
	%w	曜日 (日曜日を 0 として 0 ～ 6)
	%W	年間の経過した週の数 (最初の月曜日を 1 日目としてカウント)
	%x	ロケールの適切な日付表現 (yy/MM/dd)
	%X	ロケールの適切な時刻 (hh:mi AM/PM)
	%y	西暦の下 2 桁の年 (yy)
	%Y	西暦 (yyyy)
	%z	UTCからの時差
	%Z	タイムゾーンの名前
	%%	%
	%Ec	ロケールでの日付及び時刻の代替表現
	%EC	ロケールでの代替表現である年号
	%Ex	ロケールでの日付の代替表現
	%EX	ロケールでの時刻の代替表現
	%Ey	ロケールでの代替表現である %EC (年号) に対応した年
	%EY	ロケールでの年の完全形式の代替表現
	%Od	ロケールでの代替数値記号を使った日の表現 (必要に応じて 0 を前置)
	%Oe	ロケールでの代替数値記号を使った日の表現 (必要に応じて空白を前置)
	%OH	ロケールでの代替数値記号を使った時間 (24 時間制)
	%OI	ロケールでの代替数値記号を使った時間 (12 時間制)
	%Om	ロケールでの代替数値記号を使った月
	%OM	ロケールでの代替数値記号を使った分
	%OS	ロケールでの代替数値記号を使った秒
	%Ou	ロケールでの代替表現による暦週の中の序数 (月曜日が 1)
	%OU	ロケールでの代替数値記号を使った週番号
	%OV	ロケールでの代替数値記号を使った暦週の序数
	%Ow	ロケールでの代替数値記号を使った曜日
	%OW	ロケールでの代替数値記号を使った年の週番号
	%Oy	ロケールでの代替数値記号を使った西暦の下2桁の年
	*/
};
}//namespace atd
#endif//__atd_datetime_h__
