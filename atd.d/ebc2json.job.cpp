// ebc2json.job.cpp
#include "common.h"
#include "ebc2json.job.h"
using namespace ebc2json;
job::job(
	  const string &ebc
	, const string &fdg
	, const string &json
)
: todo(0), done(0)
, board(0)
{
	path.ebc = ebc;
	path.fdg = fdg;
	path.json = json;
	this->fdg.loadcobol(path.fdg);

	//出力ストリームクリア＆追記オープン
	ofs.ebc.open(ebc.c_str(), std::ios::out);
	ofs.ebc.close();
	ofs.ebc.open(ebc.c_str(), std::ios::app | std::ios::binary);
	ofs.json.open(json.c_str(), std::ios::out);
	ofs.json.close();
	ofs.json.open(ebc.c_str(), std::ios::app | std::ios::binary);

}
job::~job()
{
	delete board;
}
job::map::map()
: rsize(0)
, fsize(0)
{
}
job::map::~map()
{
	clear();
}
void job::map::clear()
{
	for (iterator i = begin(); i != end(); ++i)
	{
		delete i->second;
		iterator p = i;
		erase(p);
	}
}
void job::map::init(const string &ebc, const strings &fdgs, const strings &keys, const strings &jsons)
{
	//ジョブ登録
	for (strings::const_iterator 
		  f = fdgs .begin(), fe = fdgs .end()
		, k = keys .begin(), ke = keys .end()
		, j = jsons.begin(), je = jsons.end()
		; f != fe && k != ke && j != je; ++f, ++k, ++j
	)
	{
		struct { string ebc; } split;
		const string &key = *k;
		const string &fdg = *f;
		const string &json = *j;
		split.ebc = ebc + "." + key + "." + path::filename(fdg) + ".tran.json";

		//レコード長取得
		insert(value_type(key[0], new job(split.ebc, fdg, json)));
		int rsize = find(key[0])->second->fdg.rsize;
		if (rsize > this->rsize) this->rsize = rsize;
	}

	//ファイルサイズ
	fsize = path::filesize(ebc);

	//EBCDICファイルを開く
	ifs.open(ebc.c_str(), std::ios::in | std::ios::binary);
}
//====================================================
//= job::map::read() -- EBCDICファイルをレコード区分で分割する
//====================================================
void job::map::read(bool looksuffix)
{
	if (!fsize || !rsize) return;

	//プログレスバー準備
	int64 todo = fsize / rsize;
	int64 done = 0;
	int frames = 50; //★
	int64 frame = todo / frames;
	string progress;

	//EBCDICエンコダ
	string::ebcdic ebcdic;

	//EBCDICファイルを１行ずつ処理
	string line(rsize, 0);
	iterator i;
	while (ifs && ifs.read(&line[0], line.size()))
	{
		//レコード区分ってやつ
		int offset = looksuffix ? rsize-1 : 0;
		uchar ebc = line[offset];
		uchar key = ebcdic.ebc2sjis_byte(ebc);

		//ジョブ特定
		i = find(key);
		if (i == end()) continue;
		job *j = i->second;

		//TODO行数インクリメント
		j->todo++;
		//EBCDIC分割先ストリーム
		std::ofstream &ofs = j->ofs.ebc;

		//ファイルを書き込む
		ofs.write(&line[0], line.size());

		//プログレスバー！！
		if (done % frame == 0) 
		{
			progress += "#";
			cout << "\r" << progress;
			cout << flush;
		}
		//処理済み行数インクリメント
		done++;
	}
	//プログレスバー終了
	cout << endl;

	//分割EBCDICファイルのストリームをクローズ
	for (iterator i = begin(), e = end(); i != e; ++i)
	{
		i->second->ofs.ebc.close();
	}

}
//====================================================
//= job::translate() -- EBCDICファイルを変換する
//====================================================
void job::attachboard(struct board *board)
{
	this->board = board;
	board->json = path.json;
	board->todo = todo;
}
void job::notifyboard(int64 done)
{
	if (board) board->notify(done);
}
void job::translate()
{
	ifs.ebc.open(path.ebc.c_str(), std::ios::binary | std::ios::in);
	ifs.ebc.close();
}
//====================================================
//= struct job::board --オブザーバー純粋仮想クラス--
//====================================================
job::board::board()
: todo(0)
{
}
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void job::map::demo(const generic::notify &notify) const 
{
	for (const_iterator i = begin(), e = end(); i != e; ++i)
	{
		i->second->demo(notify);
	}
}
void job::demo(const generic::notify &notify) const 
{
	generic::notifyf notifyf = notify;
	notifyf(
		"*> %s %lld/%lld"
		, path::basename(path.json).c_str(), done, todo
	);
}