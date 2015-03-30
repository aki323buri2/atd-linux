// common.h
#ifndef __common_h__
#define __common_h__
#include "atd.h"
using namespace atd;
static struct app
{
	string path, dirname;
	app()
	{
		path = path::exename();
		dirname = path::dirname(path);
	}
} app;
static void notify(const string &s)
{
	cout << s << endl;
}
static generic::notifyf notifyf = notify;
#endif//__common_h__