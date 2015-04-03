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

//====================================================
//= fcheck
//====================================================
struct fcheck : public object
{
	static bool check(const string &path)
	{
		bool exists = path::exists(path, F_OK);
		notifyf("> %s : %s"
			, exists ? " OK " : "(NG)"
			, path.c_str()
		);

		return exists;
	}
};
//====================================================
#endif//__common_h__