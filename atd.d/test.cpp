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
	commandline.value_of("ebc") = home + "/ebc/ZAIKOPF.KCRDMLB";
	commandline.value_of("ebc") = home + "/ebc/URIRUISF.161";
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
	test(arg.fdg + ";" + arg.ebc);
	

	return 0;
}

#include "cobol.h"
void test(const string &text)
{
	notify("##########################################################");
	strings ss = text.explode(";");
	struct { string fdg, ebc; } path;
	path.fdg = ss[0];
	path.ebc = ss[1];

	std::ifstream ifs;
	//FDGファイル読む
	ifs.open(path.fdg.c_str(), std::ios::in);
	
	cobol::fdg fdg;
	fdg.loadcobol(ifs);
	fdg.demo(notify);


	ifs.close();

	//EBCファイルの情報取得
	notify ("");
	notifyf(">> EBCDICファイル名 : %s", path.ebc.c_str());
	notify (">> -------------------------------------------------------");
	path::fileinfo_t info = path::fileinfo(path.ebc);
	info.demo(notify);
	notify (">> -------------------------------------------------------");

	//行数計算
	string line(fdg.rsize, 0);
	int64 lines = info.size / line.size();

	notify("");
	notifyf(">> lines to do = %d", lines);

	//EBCファイルを読む
	int64 done = 0;
	ifs.open(path.ebc.c_str(), std::ios::in);

	//サンプル出力
	int pickupid = 0;
	int pickupcount = 20;//★
	int64 pickupspan = lines / pickupcount;

	//変換用スケルトン
	generic::properties conv = fdg.propskelton();
	generic::properties disp = fdg.propskelton();
	
	notify ("");
	notify (" PIC  Line  description");
	notify ("-----------------------------------------------------------");

	string::encoder utf8("UTF-8", "SJIS-WIN");

	while (ifs && ifs.read(&line[0], line.size()))
	{
		//変換実行！！
		fdg.conv(line, conv);
		for (generic::properties::iterator i = conv.begin(), e = conv.end()
			; i != e; ++i)
		{
			disp[i->name] = utf8.encode(i->value);
		}

		done++;
		if (done % pickupspan == 1)//★
		{
			notifyf(
				"%3d : "
				"%5d XXXXXXX"
				, ++pickupid
				, done
			);
			//★★★★★★★★★★★★★★★★★★★★★★★★★★★
			if (false
				// || pickupid == 1
				|| pickupid == 2
				|| pickupid == 3
			)
			{
				disp.demo(notify);
			}
			//★★★★★★★★★★★★★★★★★★★★★★★★★★★
		}
	}
	notifyf(">> lines done  = %d", done);


	notify("##########################################################");
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