// common.h
#ifndef __common_h__
#define __common_h__
#include "atd.h"
using namespace atd;
static struct app
{
	string path, dirname, basename, filename, logfile;
	app()
	{
		path = path::exename();
		dirname = path::dirname(path);
		basename = path::basename(path);
		filename = path::remove_extension(basename);
		logfile = path::rename_extension(path, ".log");
	}
} app;
static void notify(const string &s)
{
	std::ofstream ofs(app.logfile.c_str(), std::ios::app);
	ofs << "[" << datetime::now().strftime() << "] ";
	ofs << s << endl;
	cout << s << endl;
}
static generic::notifyf notifyf = notify;
int main(int argc, char **argv);
int frame(int argc, char **argv);
int run(int argc, char **argv);
//====================================================
//= struct commandline
//====================================================
struct commandline : public generic::properties
{
	bool showhelp;

	commandline()
	: showhelp(false)
	{
	}
	commandline(int argc, char **argv)
	: showhelp(false)
	{
		apply(argc, argv);
	}
	void apply(int argc, char **argv);
};
//====================================================
//=　search fdg path
//====================================================
string search_fdg(const string &name, const string &root)
{	
	//検索対象ディレクトリリスト
	strings dirs(root);
	dirs.entry(path::dirname(root) + "/fdg");//../fdg

	//ファイル名の検索対象リスト
	strings names(name);
	//"XXXX.(ライブラリ名)の場合、ライブラリ名を除いた名前も検索対象にする
	string strip = regex::replace("\\.[^\\.]+$", name, "");
	if (strip.length() < name.length()) 
	{
		names.entry(strip);
	}

	//ディレクトリリスト x ファイル名リストで検索
	struct { bool check; string path; } hit = { false, "" };
	for (strings::const_iterator i = dirs.begin(), e = dirs.end()
		; i != e; ++i)
	{
		const string &dir = *i;
		for (strings::const_iterator i = names.begin(), e = names.end()
			; i != e; ++i)
		{
			const string &name = *i;
			string path = dir + "/" + name + ".FDG.txt"; //★
			if (path::exists(path))
			{
				hit.check = true;
				hit.path = path;
				break;
			}
		}
		if (hit.check) break;
	}
	return hit.path;
}

//====================================================
//= fcheck
//====================================================
struct fcheck : public object
{
	static bool check(const string &path)
	{
		bool exists = path::exists(path);
		notifyf("> %s : %s"
			, exists ? " OK " : "(NG)"
			, path.c_str()
		);

		return exists;
	}
};
//====================================================
#endif//__common_h__