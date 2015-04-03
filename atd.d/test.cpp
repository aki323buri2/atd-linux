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
	string home = path::dirname(app.dirname);

	commandline.value_of("ebc") = home + "/ebc/TMASAPF.RDMLIB";
	commandline.apply(argc, argv);

	if (commandline.showhelp)
	{
		cerr << 
			"" CRLF
			"Usage: " << app.basename << ":"
			" [-e --ebc=<path>]" 
			" [-d --fdg=<path>]" 
			" [-j --json=<path>]" CRLF
			CRLF
			"Options: " CRLF
			"  -e --ebc=<path>    : EBCDICファイルのパス" CRLF
			"  -d --fdg=<path>    : FDG列定義ファイルのパス" CRLF
			"  -j --json=<path>   : 変換後JSONファイルのパス" CRLF
			<< endl;
		return 0;
	}

	//FDGパス自動認識
	if (!arg.fdg.length())
	{
		string path = arg.ebc;
		arg.fdg = search_fdg(
			  path::basename(path)	//対象ファイルのファイル名
			, path::dirname(path)	//対象ファイルがあるフォルダ
		);
	}

	//出力ディレクトリの作成
	string dir = app.dirname + "/tran";
	path::mkdir(dir);
	arg.json = dir + "/" + path::basename(arg.ebc) + ".tran.json";
	

	notifyf("arg.ebc  = %s", arg.ebc.c_str());
	notifyf("arg.fdg  = %s", arg.fdg.c_str());
	notifyf("arg.json = %s", arg.json.c_str());


	bool ok = true;
	ok = fcheck::check(arg.ebc) && ok;
	ok = fcheck::check(arg.fdg) && ok;
	ok = fcheck::check(arg.json) && ok;

	cout << ok << endl;

	return 0;
}
//====================================================
//= parse commandline
//====================================================
#include <getopt.h>
void commandline::apply(int argc, char **argv)
{
	struct option options[] =
	{
		{"help", no_argument		, NULL, 'h'}, 
		{"ebc" , required_argument	, NULL, 'e'}, 
		{"fdg" , required_argument	, NULL, 'd'}, 
		{"json", required_argument	, NULL, 'j'}, 
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
		int opt = ::getopt_long(argc, argv, "he:d:j:", options, &oi);
		if (opt == -1) break;

		switch (opt)
		{
		case '?':
			break;
		case 'h':
			showhelp = true;
			break;
		default: 
			value_of(map[opt]) = optarg;
			break;
		}
	}
}