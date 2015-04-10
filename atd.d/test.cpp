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

//	commandline.value_of("ebc") = home + "/ebc/TMASAPF.RDMLIB";
//	commandline.value_of("ebc") = home + "/ebc/ZAIKOPF.KCRDMLB";
	commandline.value_of("ebc") = "/home/ebcdic-files/TDBK1D1.D0209";
	commandline.value_of("fdg") = home + "/fdg/TUF010.TXT";
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
	test(arg.fdg + ";" + arg.ebc + ";" + arg.json);
	

	return 0;
}

#include "cobol.h"
#include "picojson.h"
void test(const string &text)
{	
	notify("##########################################################");
	strings ss = text.explode(";");
	struct { string fdg, ebc, json; } path;
	path.fdg  = ss[0];
	path.ebc  = ss[1];
	path.json = ss[2];

	std::ifstream ifs;
	std::ofstream ofs;

	//================================================
	//FDGファイル読む
	ifs.open(path.fdg.c_str(), std::ios::in);
	cobol::fdg fdg;
	fdg.loadcobol(ifs);
	fdg.demo(notify);
	ifs.close();
	//================================================

	//EBCファイルの情報取得
	notify ("");
	notifyf(">> EBCDICファイル名 : ");
	notifyf(">> %s", path.ebc.c_str());
	notify (">> -------------------------------------------------------");
	path::fileinfo_t info = path::fileinfo(path.ebc);
	info.demo(notify);
	notify (">> -------------------------------------------------------");


	//行数計算
	string line(fdg.rsize, 0);
	int64 lines = info.size / line.size();

	notify("");
	notifyf(">> lines todo = %d", lines);
	return;

	//変換用スケルトン作成
	generic::properties conv = fdg.propskelton();
	generic::properties disp = conv;//コピー

	//UTF-8エンコーダ作成
	string::encoder utf8enc("UTF-8", "SJIS-WIN");
	
	notify ("");

	//EBCファイルをオープン
	ifs.open(path.ebc.c_str(), std::ios::in | std::ios::binary);

	//JSONファイル（出力）オープン
	ofs.open(path.json.c_str(), std::ios::out);

	ofs << "[\n";

	int64 done = 0;
	while (ifs && ifs.read(&line[0], line.size()))
	{
		//--------------------------------------------
		//変換実行！！
		fdg.conv(line, conv);
		//--------------------------------------------

		ofs << (done++ ? "," : " ");
		ofs << "{\n";

		int c = 0;
		for (generic::properties::iterator i = conv.begin(), e = conv.end()
			; i != e; ++i)
		{
			ofs << (c++ ? "," : " ");
			//UTF-8変換
			string name = utf8enc.encode(i->name);
			string value = utf8enc.encode(i->value);
			const char *dq = DOUBLE_QUOTATION;
			ofs << dq << string::jsonescape(name) << dq;
			ofs << ": ";
			ofs << dq << string::jsonescape(value) << dq;
			ofs << "\n";
		}

		ofs << "}\n";
	}
	
	ofs << "]";
	ofs.close();
	ifs.close();
	
	notifyf(">> lines done  = %d", done);

	notify ("");
	notifyf(">> JSONファイル名 : ");
	notifyf(">> %s", path.json.c_str());
	notify (">> -------------------------------------------------------");
	info = path::fileinfo(path.json);
	info.demo(notify);
	notify (">> -------------------------------------------------------");

	//JSONの読み込みテスト
	ifs.open(path.json.c_str(), std::ios::in);
	picojson::value v(picojson::object_type, false);
	ifs >> v;

	generic::properties read = disp;//コピー

	int64 rr = 0;
	picojson::array &a = v.get<picojson::array>();
	for (picojson::array::iterator i = a.begin(), e = a.end()
		; i != e; ++i)
	{
		picojson::object &o = i->get<picojson::object>();
		for (picojson::object::iterator i = o.begin(), e = o.end()
			; i != e; ++i)
		{
			read.value_of(i->first) = i->second.to_str();
		}
		rr++;	

		if (rr == 7000)
		{
			read.demo(notify);
		}	
	}
	cout << rr << endl;



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