// common.h
#ifndef __common_h__
#define __common_h__
#include "atd.h"
using namespace atd;

void test(const string &text);//★

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
string search_fdg(const string &path)
{	
	string name = path::basename(path);
	string root = path::dirname(path);
	//検索対象ディレクトリリスト
	strings dirs(root);
	dirs.entry(path::dirname(root) + "/fdg");//../fdg

	//ファイル名の検索対象リスト
	strings names(name);

	//名前と修飾子（ライブラリ名）に分割
	struct { string name, lib; } split;
	strings match;
	if (regex::match("^(.+)\\.([^\\.]+)$", name, match))
	{
		split.name = match[1];
		split.lib  = match[2];
		names.entry(split.name);
	}
	else 
	{
		split.name = name;
		split.lib  = "";
	}
	//RDMLIBは特別！★
	const string abslib = "RDMLIB";
	if (split.lib != abslib) names.entry(split.name + "." + abslib);


	//ディレクトリリスト x ファイル名リストで検索
	struct { bool check; string path; } hit = { false, "" };
	for (strings::const_iterator i = dirs.begin(), e = dirs.end()
		; i != e; ++i)
	{
		//ディレクトリ名
		const string &dir = *i;

		for (strings::const_iterator i = names.begin(), e = names.end()
			; i != e; ++i)
		{
			//ファイル名
			const string &name = *i;

			//存在チェック
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