//atd.path.cpp
#include "atd.h"
using namespace atd;
#include <libgen.h>//for ::dirname(), ::basename()
#include <sys/stat.h>	//for ::mkdir()
#include <errno.h>
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
//拡張子を除いたファイル名
string path::filename(const string &path)
{
	return remove_extension(basename(path));
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
//----------------------------------------------------
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
//シンボリックリンクかどうか
bool path::islink(const string &path)
{
	struct stat st = {0};
	if (::stat(path.c_str(), &st) != 0) return false;
	return (st.st_mode & S_IFLNK) == S_IFDIR;
}
//ファイルサイズ
int64 path::filesize(const string &path)
{
	return fileinfo(path).size;
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
namespace
{
	static bool error(const char *format, ...)
	{
		int err = errno;

		va_list va;
		va_start(va, format);
		string s(::vsnprintf(0, 0, format, va) + 1, 0);
		va_end(va);
		va_start(va, format);
		::vsnprintf(&s[0], s.size(), format, va);
		va_end(va);

		std::cerr << "!!! errno[" << err << "] " <<  s << endl;
		return false;
	}
}
//ファイルコピー＆移動
bool path::copy(const string &from, const string &to, bool force)
{
	if (!force && exists(to))
	{
		return error("path::copy() : %s が存在しています", to.c_str());
	}
	std::ifstream ifs(from.c_str(), std::ios::binary | std::ios::in);
	std::ofstream ofs(to  .c_str(), std::ios::binary | std::ios::out);
	if (filesize(from))
	{
		ofs << ifs.rdbuf() << std::flush;
	}
	return (ifs && ofs);

}
bool path::move(const string &from, const string &to, bool force)
{
	bool ex = exists(to);
	if (!force && ex)
	{
		return error("path::move() : %s が存在しています", to.c_str());
	}
	if (ex && !unlink(to))
	{
		return false;
	}
	//リネームトライ
	bool success = (::rename(from.c_str(), to.c_str()) == 0);
	if (!success && errno != EXDEV)
	{
		return error("path::move() : rename失敗 %s -> %s", from.c_str(), to.c_str());
	}
	if (!success)
	{
		//マウント違いで失敗してるんならバイトコピーしてコピー元削除
		success = copy(from, to) && unlink(from);
	}
	return success;
}
//ファイルの削除
bool path::unlink(const string &path)
{
	if (::unlink(path.c_str()) != 0)
	{
		return error("path::unlink() : %s の削除に失敗", path.c_str());
	}
	return true;
}
//一時ファイルの作成
string path::mkstemp(const string &prefix)
{
	string flex = "XXXXXX";//'X'６桁で指定するのはmkstemp()システムコールの仕様です！
	string s = prefix + "-" + flex;
	int fd = ::mkstemp(&s[0]);//ファイルデスクリプタが返る
	if (fd < 0)
	{
		error("path::mkstemp() : 一時ファイルの作成に失敗 template='%s'", s.c_str());
		return "";
	}
	return s.c_str();
}
//====================================================
//= ファイル情報
//====================================================
path::fileinfo_t::fileinfo_t()
: dev(0)
, ino(0)
, mode(0)
, nlink(0)
, uid(0)
, gid(0)
, rdev(0)
, size(0)
, blksize(0)
, blocks(0)
{
	atime = datetime::zero();
	mtime = datetime::zero();
	ctime = datetime::zero();
}
path::fileinfo_t path::fileinfo(const string &path)
{
	fileinfo_t info;

	struct stat st = {0};
	if (::stat(path.c_str(), &st) != 0) return info;

	info.dev	= st.st_dev		;/* ファイルがあるデバイスの ID */
	info.ino	= st.st_ino		;/* inode 番号 */
	info.mode	= st.st_mode	;/* アクセス保護 */
	info.nlink	= st.st_nlink	;/* ハードリンクの数 */
	info.uid	= st.st_uid		;/* 所有者のユーザー ID */
	info.gid	= st.st_gid		;/* 所有者のグループ ID */
	info.rdev	= st.st_rdev	;/* デバイス ID (特殊ファイルの場合) */
	info.size	= st.st_size	;/* 全体のサイズ (バイト単位) */
	info.blksize= st.st_blksize	;/* ファイルシステム I/O でのブロックサイズ */
	info.blocks	= st.st_blocks	;/* 割り当てられた 512B のブロック数 */

	info.atime.fromtm(*::localtime(&st.st_atim.tv_sec));
	info.mtime.fromtm(*::localtime(&st.st_mtim.tv_sec));
	info.ctime.fromtm(*::localtime(&st.st_ctim.tv_sec));


	return info;
}
void path::fileinfo_t::demo(const generic::notify &notify)
{
	int offset = 20;
	generic::notifyf notifyf = notify;
	notifyf("dev      : %*d %s ", offset, dev		, "/* ファイルがあるデバイスの ID */");
	notifyf("ino      : %*d %s ", offset, ino		, "/* inode 番号 */");
	notifyf("mode     : %*d %s ", offset, mode		, "/* アクセス保護 */");
	notifyf("nlink    : %*d %s ", offset, nlink		, "/* ハードリンクの数 */");
	notifyf("uid      : %*d %s ", offset, uid		, "/* 所有者のユーザー ID */");
	notifyf("gid      : %*d %s ", offset, gid		, "/* 所有者のグループ ID */");
	notifyf("rdev     : %*d %s ", offset, rdev		, "/* デバイス ID (特殊ファイルの場合) */");
	notifyf("size     : %*d %s ", offset, size		, "/* 全体のサイズ (バイト単位) */");
	notifyf("blksize  : %*d %s ", offset, blksize	, "/* ファイルシステム I/O でのブロックサイズ */");
	notifyf("blocks   : %*d %s ", offset, blocks	, "/* 割り当てられた 512B のブロック数 */");

	notifyf("atime    : %-*s %s", offset, atime.strftime().c_str(), "/* 最終アクセス時刻 */");
	notifyf("mtime    : %-*s %s", offset, mtime.strftime().c_str(), "/* 最終修正時刻 */");
	notifyf("ctime    : %-*s %s", offset, ctime.strftime().c_str(), "/* 最終状態変更時刻 */");
}