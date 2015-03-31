//atd.path.h
#ifndef __atd_path_h__
#define __atd_path_h__
#include "atd.h"
namespace atd {;;
//====================================================
//= struct atd::path
//====================================================
struct path : public object
{
	static string dirname(const string &path);
	static string basename(const string &path);

	//カレントディレクトリ取得
	static string cwd();
	//実行ファイルのフルパス取得
	static string exename();

	//拡張子の変更
	static string remove_extension(const string &path);
	static string rename_extension(const string &path, const string &extension);

	//ファイルの有無
	static bool exists(const string &path, int mode = F_OK | R_OK);
};
}//namespace atd
#endif//__atd_path_h__