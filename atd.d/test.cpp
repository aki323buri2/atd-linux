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
	string home = path::dirname(app.dirname);

	struct commandline commandline;
	struct { string &ebc, &fdg, &json; } arg = 
	{
		commandline.value_of("ebc"), 
		commandline.value_of("fdg"), 
		commandline.value_of("json"), 
	};
	//コマンドライン引数解析
	notify("... option parsing ...");

	commandline.value_of("ebc") = home + "/ebc/TMASAPF.RDMLIB";
	commandline.apply(argc, argv);

	//Usage!
	if (commandline.showhelp)
	{
		cerr << 
			"" CRLF
			"Usage: " << app.basename << ":"
			" [-e --ebc=<path>]" 
			" [-d --fdg=<path>]" 
			CRLF
			"Options: " CRLF
			"  -e --ebc=<path>    : EBCDICファイルのパス" CRLF
			"  -d --fdg=<path>    : FDG列定義ファイルのパス" CRLF
			<< endl;
		return 1;
	}

	//FDGパスが指定されていない場合は自動認識
	if (!arg.fdg.length())
	{
		string path = arg.ebc;
		arg.fdg = search_fdg(path);
	}
	if (!arg.fdg.length())
	{
		//仮（エラー表示用といってもよい）
		arg.fdg = arg.ebc + ".FDG.txt";
	}

	//出力ディレクトリの作成
	struct { string dir; } tran;
	tran.dir = app.dirname + "/tran";
	path::mkdir(tran.dir);
	arg.json = tran.dir + "/" + path::basename(arg.ebc) + ".tran.json";

	//コマンドライン引数のディスプレイ
	notify("---- command line options    ----");
	commandline.demo(notify, 7);

	//必須ファイルorディレクトリの存在チェック
	notify("---- file or directory check ----");
	bool ok = true;
	ok = fcheck::check(arg.ebc) && ok;
	ok = fcheck::check(arg.fdg) && ok;
	ok = fcheck::check(tran.dir) && ok;

	if (!ok)
	{
		notify("!!!! 必要なファイルもしくはディレクトリが存在しません !!!!");
		return 1;
	}

	// test(arg.fdg);
	test(arg.ebc);
	

	return 0;
}
#include <iconv.h>
#include <errno.h>
void test(const string &text)
{
	notify("######################################################");
	string path = text;

	// iconv_t ic = ::iconv_open("UTF-8", "SJIS");
	iconv_t ic = ::iconv_open("SJIS", "EBCDIC-JP-KANA");


	std::ifstream ifs(path.c_str(), std::ios::in);
	string line;
	string conv(0x100, 0);
	while (ifs && std::getline(ifs, line))
	{

		conv.assign(line.size() * 2, 0);

		struct { char *line, *conv; } pt = { &line[0], &conv[0] };
		struct { size_t line, conv; } sz = { line.size(), conv.size() };

		sz.line = 20;
		sz.conv = 20;
		int r = ::iconv(ic
			, &pt.line, &sz.line
			, &pt.conv, &sz.conv
		);
		int e = errno;
		cout << "e:" << e;
		cout << "r:" << r;
		cout << " - " << conv.c_str();
		cout << endl;

	}
	::iconv_close(ic);
	cout << "E2BIG : " << E2BIG << endl;
	cout << "EILSEQ : " << EILSEQ << endl;
	cout << "EINVAL : " << EINVAL << endl;

	notify("######################################################");
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
		{0}
	};
	std::map<int, string> map;
	for (struct option *o = options; o->name; o++)
	{
		map[o->val] = o->name;
	}

	showhelp = false;

	while (true)
	{
		int oi;
		int opt = ::getopt_long(argc, argv, "he:d:", options, &oi);
		if (opt == -1)
		{
			break;
		}

		switch (opt)
		{
		case '?':
		case 'h':
			showhelp = true;
			break;
		default: 
			value_of(map[opt]) = optarg;
			break;
		}
	}
}