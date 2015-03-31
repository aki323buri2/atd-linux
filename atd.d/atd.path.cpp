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
//※<libgen.h>でbasenameを__xpg_basenameに#defineしている！！
//  なので強引な解決策（なんか他にないのんか？）  
#define __xpg_basename basename
string path::basename(const string &path)
#define basename __xpg_basename
{
	string s = path; 

	//パスの最後に'/'があると正常に動かないバグを回避
	if (s.endswith("/"))
	{
		s = regex::replace(s, "/$", "");
	}

	return ::basename(&s[0]);
}
//カレンドディレクトリ取得
string path::cwd()
{
	// POSIX.1-2001 標準の拡張として、 glibc では buf が NULL の場合、 
	// getcwd() は必要なバッファーを malloc(3) を用いて動的に割り当てる。 
	// この場合、 size が 0 の場合を除き、バッファーの長さは size となる。 
	// size が 0 の場合には必要な大きさが確保される。 
	// 呼び出し側で、返されたバッファーを free(3) すべきである。
	// ...だってさ
	char *p = ::getcwd(0, 0);
	if (!p) return "";

	string s = p;
	free(p);
	return s;
}
//実行ファイルのフルパス
string path::exename()
{
	// パクリ元 : http://d.hatena.ne.jp/iww/20120804/readlink 
	// /proc/数字/exe が実行ファイルのパスになってるので、それを読むだけ。
	string s(0x400, 0);//★
	::readlink("/proc/self/exe", &s[0], s.size());
	return s.c_str();
}
//拡張子の消去
string path::remove_extension(const string &path)
{
	string s = regex::replace(path, "/$", "");
	size_t pos = s.find_last_of(".");
	return s.substr(0, pos);
}
//拡張子の変更
string path::rename_extension(const string &path, const string &extension)
{
	return remove_extension(path) + extension;
}