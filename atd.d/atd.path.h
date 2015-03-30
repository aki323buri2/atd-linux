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
};
}//namespace atd
#endif//__atd_path_h__