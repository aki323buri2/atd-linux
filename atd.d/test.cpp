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

#include <getopt.h>
int run(int argc, char **argv) 
{
	string sentence, suffix;
	struct option options[] = {
		{"morning"	, no_argument		, NULL, 'm'}, 
		{"evening"	, no_argument		, NULL, 'e'}, 
		{"suffex"	, required_argument	, NULL, 's'}, 
		{0}, 
	};
	while (true)
	{
		int oi;
		int opt = ::getopt_long(argc, argv, "mes:", options, &oi);
		if (opt == -1) break;

		switch (opt)
		{
		case 'm': 
			sentence = "good morning";
		case 'e': 
			sentence = "good evening";

			cout << "option : " << (char)opt << endl;
			break;
		case 's':
			suffix = optarg;
			cout << "option : " << (char)opt << " = " << optarg << endl;
			break;
		case '?':
			//解析できないオプションが見つかった場合は「?」を返す
			//オプション引数が不足している場合も「?」を返す
			cout << "!! Unknown or required argument option : " << (char)optopt << endl;
			cout << "Usage: COMMAND [-m | -e] [-s suffix] name ..." << endl;
			return 1;
		}
	}

	for (int i = optind; i < argc; i++)
	{
		cout << sentence << ", " << argv[i] << suffix << "!" << endl;
	}

	return 0;
} 