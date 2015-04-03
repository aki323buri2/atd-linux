//atd.path.cpp
#include "atd.h"
using namespace atd;
#include <libgen.h>//for ::dirname(), ::basename()
#include <sys/stat.h>	//for ::mkdir()
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
	string s = regex::replace("/$", path, "");
	size_t pos = s.find_last_of(".");
	return s.substr(0, pos);
}
//拡張子の変更
string path::rename_extension(const string &path, const string &extension)
{
	return remove_extension(path) + extension;
}
//ファイルもしくはディレクトリの有無
bool path::exists(const string &path)
{
	struct stat st = {0};
	return ::stat(path.c_str(), &st) == 0;
}
// S_IFMT	0170000	ファイル種別を示すビット領域を表すビットマスク
//----------------------------------------------------
// S_IFSOCK	0140000	ソケット
// S_IFLNK	0120000	シンボリックリンク
// S_IFREG	0100000	通常のファイル
// S_IFBLK	0060000	ブロックデバイス
// S_IFDIR	0040000	ディレクトリ
// S_IFCHR	0020000	キャラクターデバイス
// S_IFIFO	0010000	FIFO
//ファイルかどうか
bool path::isfile(const string &path)
{
	struct stat st = {0};
	if (::stat(path.c_str(), &st) != 0) return false;
	return (st.st_mode & S_IFMT) == S_IFREG;
}
//ディレクトリかどうか
bool path::isdir(const string &path)
{
	struct stat st = {0};
	if (::stat(path.c_str(), &st) != 0) return false;
	return (st.st_mode & S_IFMT) == S_IFDIR;
}
//ディレクトリかどうか
bool path::islink(const string &path)
{
	struct stat st = {0};
	if (::stat(path.c_str(), &st) != 0) return false;
	return (st.st_mode & S_IFLNK) == S_IFDIR;
}
//ディレクトリの作成
bool path::mkdir(const string &path, int mode)
{
	if (isdir(path)) return true;
	
	string s = regex::replace("^\\/", path, "");
	strings dirs = s.explode("/");
	string dir = "";
	int r = 0;
	for (strings::iterator i = dirs.begin(), e = dirs.end()
		; i != e; ++i)
	{
		dir += "/";
		dir += *i;
		if (isdir(dir)) continue;
		r = ::mkdir(dir.c_str(), mode);
		cout << r << " : " << dir << endl;
	}
	//最終的に成功したかどうか
	return r == 0;
}