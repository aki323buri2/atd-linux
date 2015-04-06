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
		arg.fdg = search_fdg(arg.ebc);
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

	//出力JSONのパス
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
	test(arg.fdg);
	

	return 0;
}

void test(const string &text)
{
	notify("######################################################");
	string path = text;

	string pattern = 
		"^([0-9]{6})?"		//DUETで作ったファイルの行番号
		"\\s*([0-9]{2})" 	//LV
		"\\s+([^ ]+)"		//識別名
		"("
			"\\s+PIC\\s+"
			"(S?)"						//符号付き？
			"(9|X|N)"					//タイプ
			"\\(([0-9]+)\\)"			//桁数
			"(V9\\(([0-9]+)\\)|V(9+))?"	//小数点以下
		")?"
		"(\\s+(PACKED-DECIMAL|COMP-3))?"	//パック項目
		"(\\s+OCCURS\\s+([0-9]+))?"			//OCCURS
		;
	regex re;
	re.compile(pattern);

	//ファイル読む
	std::ifstream ifs(path.c_str(), std::ios::in);
	string::encoder enc("UTF-8", "SJIS-WIN");

	string line;
	while (ifs && std::getline(ifs, line))
	{
		strings match = re.match(line);
		if (match.size() <= 1) continue;

		int lv		= match[ 2].toint();
		string name	= match[ 3];
		bool sig	= match[ 5].length();
		string type	= match[ 6];
		int left	= match[ 7].toint();
		int right	= 0;
		bool pack	= match[11].length();
		int occurs	= match[14].toint();

		string s = match[8];
		string n = match[9];
		if (s.find('(') != string::npos)
		{
			//V99...
			right = n.length();
		}
		else
		{
			//V9(NN)
			right = n.toint();
		}

		cout << string::format(
			"%02d %-10s %1s %-10s %-10s"
			, lv, name.c_str(), type.c_str()
			, left ? string::format("(%3d-%d)", left, right).c_str() : ""
			, occurs ? string::format("OCCURS %2d", occurs).c_str() : ""
			) << endl;

		
	}
	

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