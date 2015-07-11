// main.cpp
#include "common.h"
#include "ebc2json.h"
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
	//ホームディレクトリ
	string home = path::dirname(app.dirname);

	//コマンドライン引数オブジェクト
	struct commandline commandline;
	struct { string &ebc, &fdg, &keys; } arg = 
	{
		commandline.value_of("ebc"), 
		commandline.value_of("fdg"), 		
		commandline.value_of("keys"), 
	};
	

	//-----マルチレコードパターン-----//
	commandline.value_of("ebc") = home + "/ebc/TDBK1D1.D0209";
	commandline.value_of("ebc") = home + "/ebc/TDBK1D1.D0419";
	commandline.value_of("ebc") = home + "/ebc/TDBK1D1.D0419";
	commandline.value_of("fdg") = 
		""  + home + "/fdg/TUF010.TXT"
		";" + home + "/fdg/TUF020.TXT"
		";" + home + "/fdg/TUF090.TXT"
		;
	commandline.value_of("keys") = "1,2,3";
	//-----マルチレコードパターン-----//
	
	//-----シングルレコードパターン-----//
	commandline.value_of("ebc") = home + "/ebc/TMASAPF.RDMLIB";
	commandline.value_of("ebc") = home + "/ebc/ZAIKOPF.KCRDMLB";
	commandline.value_of("ebc") = home + "/ebc/KANSOLSF.NAKAFIL1";
	commandline.value_of("fdg") = "";
	commandline.value_of("keys") = "1";
	//-----シングルレコードパターン-----//

	//コマンドライン引数解析
	commandline.apply(argc, argv);

	//Usage!
	if (commandline.showhelp)
	{
		cerr << 
			"" CRLF
			"Usage: " << app.basename << " "
			"[Options]" 
			CRLF
			"Options: " CRLF
			"  -h --help -?               : ヘルプの表示" CRLF
			"  -e --ebc=<path>            : EBCDICファイルのパス" CRLF
			"  -d --fdg=<path[;path2;..]> : FDG列定義ファイルのパス（リストも可-';'区切り-）" CRLF
			"  -k --keys=<key1,key2,>     : マルチレコードの判別文字リスト-','区切り- *1" CRLF
			CRLF
			"                             注)'-d' および '-k'オプションに区切り文字を指定して複数の値を設定する場合、" CRLF
			"                                 '\"'で文字列を囲む必要があります" CRLF
			CRLF
			"  -s --looksuffix            : マルチレコードの判別文字が末尾かどうか (規定値 false)" CRLF
			"  -t --tmpatlocal            : 一時ファイルを/tmpではなく出力フォルダに作るかどうか (規定値false)" CRLF
			CRLF
			"                             注)一時ファイルを/tmpではなく出力フォルダに作ると、" CRLF
			"                                 「物理メモリの半分のサイズ」という/tmpの制限を超えることができますが、" CRLF
			"                                 処理スピードが遅くなるかもしれません。" CRLF
			CRLF
			<< endl;
		return 1;
	}

	//FDGが空白ならばEBCファイル名を元に所定の場所探す
	if (!arg.fdg.length())
	{
		arg.fdg = searchfdg::search(arg.ebc);
	}

	//コマンドライン引数のディスプレイ
	notify("");
	notify("---- command line options    ----");
	commandline.demo(notify);

	//引数チェック
	if (!arg.fdg.length())
	{
		notify("FDGが見つかりません");
		return 1;
	}

	//パラメータの配列化
	struct { strings fdg, key, json; } list;
	list.fdg = arg.fdg .explode(";");
	list.key = arg.keys.explode(",");

	//マルチレコード要素数チェック
	if (list.fdg.size() && (list.fdg.size() > list.key.size()))
	{
		notify("判別キー配列の要素数が不足しています");
		return 1;
	}

	//出力ディレクトリの作成
	struct { string dir; } tran;
	tran.dir = app.dirname + "/tran";
	path::mkdir(tran.dir);

	//必須ファイルorディレクトリの存在チェック
	notify("");
	notify("---- file or directory check ----");
	bool ok = true;
	ok = fcheck::check(arg.ebc ) && ok;
	ok = fcheck::check(list.fdg) && ok;
	ok = fcheck::check(tran.dir) && ok;

	if (!ok)
	{
		notify("!!!! 必要なファイルもしくはディレクトリが存在しません !!!!");
		return 1;
	}

	//出力JSONのパス
	for (strings::iterator b = list.fdg.begin(), i = b, e = list.fdg.end()
		; i != e; ++i)
	{
		list.json.push_back(
			tran.dir 
			+ "/" + path::basename(arg.ebc) 
			+ "."+ list.key[i - b]
			+ "." + path::remove_extension(path::basename(*i))
			+ ".tran.json");
	}
	cout << list.json.implode("\n") << endl;

	notify("");
	notify("#################################################");
	ebc2json::execute::parallel(
		arg.ebc
		, list.fdg
		, list.key
		, list.json
		, commandline.looksuffix
		, commandline.tmpatlocal
		);
	notify("#################################################");

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
		{"keys", required_argument	, NULL, 'k'}, 
		{"look-suffix", no_argument	, NULL, 's'}, 
		{"tmp-at-local", no_argument, NULL, 't'}, 
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
		int opt = ::getopt_long(argc, argv, "he:d:k:st", options, &oi);
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
		case 's':
			looksuffix = true;
			break;
		case 't':
			tmpatlocal = true;
			break;
		default: 
				cout << optarg << endl;
			value_of(map[opt]) = optarg ? optarg : "";
			break;
		}
	}
}