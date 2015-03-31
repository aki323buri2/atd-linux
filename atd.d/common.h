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
#endif//__common_h__