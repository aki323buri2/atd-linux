// test.cpp
#include "test.job.h"


//####################################################
void test(
	string ebc
	, const strings &fdgs
	, const strings &keys
	, const strings &jsons
	, bool looksuffix
)
{
	//ジョブのマップ
	job::map jj;

	int rsize = 0;
	for (strings::const_iterator b = fdgs.begin(), e = fdgs.end(), i = b
		; i != e; ++i)
	{
		size_t offset = i - b;
		const string &fdg = *i;
		const string &key  = keys [offset].substr(0, 1);
		const string &json = jsons[offset];

		//分割用EBCDICファイルパスの生成
		struct { string ebc; } make;
		make.ebc = 
			path::dirname (json) + "/" + 
			path::basename(ebc ) + "." + key + "." + 
			path::filename(fdg )
			;
		//マップに登録
		uchar c = key[0];
		job *j = new job(make.ebc, fdg, json);
		jj[c] = j;
		//FDGロード
		j->fdgload();
		//分割EBCDICをクリアし追記モードで開いておく
		j->ebcclear();
		j->ebcopenappend();

		//レコードサイズ
		int rsizenew = j->fdg.rsize;
		if (rsizenew > rsize) rsize = rsizenew;
	}

	//EBCDICファイル分析
	path::fileinfo_t fi = path::fileinfo(ebc);
	notifyf("--- %s ---", ebc.c_str());
	fi.demo(notify);
	//行数計算
	int64 todo = fi.size / rsize;
	int64 done = 0;
	notifyf(">> todo : %d lines", todo);

	//進捗バー
	progressbar progress;
	progress.coordinate(todo, 50);
	//判定キャラクタの位置
	int offset = looksuffix ? rsize - 1 : 0;
	//EBCDICエンコーダ
	string::ebcdic ebcdic;
	//EBCDICファイルオープン
	std::ifstream ifs;
	ifs.open(ebc.c_str(), std::ios::binary | std::ios::in);

	//EBCDICファイル振り分け処理
	//レコードサイズごと処理
	string line(rsize, 0);
	while (ifs && ifs.read(&line[0], line.size()))
	{
		//判定キャラクタ
		uchar c = ebcdic.ebc2sjis_byte((uchar)line[offset]);
		//ジョブを選択
		job *j = jj[c];
		//ファイルに追記
		std::ofstream &ofs = j->ofs;
		ofs.write(&line[0], line.size());
		//TODO行数インクリメント
		j->todo++;
		//進捗
		progress.indicate(done);
		done++;
	}
	progress.finish();
	notifyf(">> done : %d lines", done);

	for (job::map::iterator i = jj.begin(), e = jj.end()
		; i != e; ++i)
	{
		uchar c = i->first;//判別キャラクタ
		job *j = i->second;//該当ジョブ
		//分割EBCDICファイルをクローズ
		std::ofstream &ofs = j->ofs;
		ofs.close();
		//TODO行数
		int64 todo = j->todo;
		//分割EBCDICファイルのサイズ
		string ebc = j->path.ebc;
		int64 size = path::filesize(ebc);

		notifyf("*> '%c' : %s has %10lld lines, %10lld bytes "
			, c
			, path::basename(ebc).c_str()
			, todo
			, size
		);
	}

	//それぞれのスレッドで並列変換実行
	jj.invoke_ebcdecode();

}
//####################################################
