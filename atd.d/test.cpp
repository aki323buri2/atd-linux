// test.cpp
#include "common.h"

void sleep(int milliseconds)
{
	int base = 1000; 
	timespec ts = { milliseconds / base, milliseconds % base };
	::nanosleep(&ts, 0); 
}

int main(int argc, char **argv)
{
	int r = 0;
	string err;
	try
	{
		r = frame(argc, argv);
	}
	catch (std::exception &e)
	{
		notifyf("!!!! %s error (%s) : %s"
			, app.basename.c_str()
			, datetime::now().strftime().c_str()
			, err.c_str()
		);
		return 1; 
	}
	return r;
}
int frame(int argc, char **argv)
{
	string name = app.filename;
	notifyf(">>>> %s start (%s) >>>>", name.c_str(), datetime::now().strftime().c_str());
	int r = run(argc, argv);
	notifyf(">>>> %s e n d (%s) >>>>", name.c_str(), datetime::now().strftime().c_str());
	notify ("");
	return r;
}

int run(int argc, char **argv)
{
	struct commandline commandline;
	struct { string &ebc, &fdg, &json; } arg = 
	{
		commandline.value_of("ebc"), 
		commandline.value_of("fdg"), 
		commandline.value_of("json"), 
	};
	commandline.get(argc, argv);

	generic::properties backup;
	backup.value_of("ebc") = "../ebc/TMASAPF.RDMLIB.FDG.txt";

	commandline.supply(backup);

	commandline.demo(notify);

	return 0;
}
//====================================================
//= parse commandline
//====================================================
#include <getopt.h>
void commandline::get(int argc, char **argv)
{
	struct option options[] =
	{
		{"ebc", required_argument, NULL, 'e'}, 
		{"fdg", required_argument, NULL, 'd'}, 
		{"json", required_argument, NULL, 'j'}, 
		{0}
	};
	std::map<int, string> map;
	for (struct option *o = options; o->name; o++)
	{
		map[o->val] = o->name;
	}

	while (true)
	{
		int oi;
		int opt = ::getopt_long(argc, argv, "e:d:j:", options, &oi);
		if (opt == -1) break;

		switch (opt)
		{
		case '?':
			break;
		default: 
			value_of(map[opt]) = optarg;
			break;
		}
	}
}