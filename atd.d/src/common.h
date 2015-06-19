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
		logfile = path + ".log";
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
	bool looksuffix;

	commandline()
	: showhelp(false)
	, looksuffix(false)
	{
	}
	commandline(int argc, char **argv)
	: showhelp(false)
	, looksuffix(false)
	{
		apply(argc, argv);
	}
	void apply(int argc, char **argv);

	void demo(const generic::notify &notify)
	{
		generic::notifyf notifyf = notify;
		generic::properties::demo(notify, 7);//★
		notifyf("*> look-suffix : %s", looksuffix ? "true" : "false");
	}
};
//====================================================
//=　search fdg path
//====================================================
struct searchfdg
{
	static string search(const string &path)
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
};
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
	static bool check(const strings &ss)
	{
		bool ok = true;
		for (strings::const_iterator i = ss.begin(), e = ss.end()
			; i != e; ++i)
		{
			ok = check(*i) && ok;
		}
		return ok;
	}
};
//====================================================
//= progressbar
//====================================================
struct progressbar : public object
{
	int64 todo, done;
	int steps, block;
	string progress, unit;
	progressbar()
	: todo(0), done(0), steps(1), block(0)
	, unit("#")
	{
	}
	void coordinate(int64 todo, int steps)
	{
		this->todo = todo;
		this->steps = steps;
		block = todo / (!steps ? 1 : steps);
	}
	void reset()
	{
		progress = "";
	}
	void indicate(int64 done)
	{
		if (done % block) return;
		progress += unit;
		cout << "\r" << progress << flush;
	}
	void finish()
	{
		cout << endl;
	}
};
#endif//__common_h__