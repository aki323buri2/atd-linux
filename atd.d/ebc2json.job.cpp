// ebc2json.job.cpp
#include "common.h"
#include "ebc2json.job.h"
using namespace ebc2json;
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
		split.ebc = 
			  path::dirname(json)
			+ path::basename(ebc) + "." + key + "." + path::filename(fdg);

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
	ofs.ebc.open(ebc.c_str(), std::ios::out | std::ios::binary);
	ofs.json.open(json.c_str(), std::ios::out | std::ios::binary);

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
	if (!frame) frame = todo;//ゼロ除算防止！
	string progress;

	//EBCDICエンコダ
	string::ebcdic ebcdic;

	//################################################
	//# FDGマップ件数が１件ならばレコード分割しなくてよい
	//################################################
	bool notest = size() == 1;

	//レコード区分のオフセット
	int offset = looksuffix ? rsize-1 : 0;

	//EBCDICファイルを１行ずつ処理
	string line(rsize, 0);
	//判定の必要が無ければ先頭要素でよい	
	iterator i = begin();
	while (ifs && ifs.read(&line[0], line.size()))
	{
		if (!notest)
		{
			//レコード区分ってやつ
			uchar ebc = line[offset];
			uchar key = ebcdic.ebc2sjis_byte(ebc);

			//ジョブ特定
			i = find(key);
			if (i == end()) continue;
		}
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
	if (!todo) return;

	//変換データ格納用スケルトン作成
	generic::properties conv = fdg.propskelton();

	//SJIS->UTF-8エンコーダ
	int buffersize = 0x400;//★
	string::encoder encoder("UTF-8", "SJIS-WIN");
	struct { 
		string name, value;
		string::encoder &enc;
	} utf8 = 
	{
		string(buffersize, 0), 
		string(buffersize, 0), 
		encoder, 
	};

	struct the
	{
		static void jsonescape(char *src, char *tar)
		{
			uchar *s = (uchar *)src;
			uchar *t = (uchar *)tar;
			for (; *s; s++)
			{
				switch (*s)
				{
				case '"' : *(t++) = '\\'; *(t++) = '"';  break;
				case '/' : *(t++) = '\\'; *(t++) = '/';  break;
				case '\\': *(t++) = '\\'; *(t++) = '\\'; break;
				default  : *(t++) = *s; break;
				}
			}
			*t = '\0';
		}
	};
	string jname(buffersize, 0);
	string jvalue(buffersize, 0);


	//通知単位
	int frames = 20;//★
	int64 frame = todo / frames;
	if (!frame) frame = todo;//ゼロ除算防止！


	//EBCDICファイルオープン
	std::ifstream &ifs = this->ifs.ebc;
	ifs.open(path.ebc.c_str(), std::ios::binary | std::ios::in);
	done = 0;

	//出力ストリーム
	std::ofstream &ofs = this->ofs.json;

	ofs << "[" << endl;

	//１行ずつ
	int rsize = fdg.rsize;
	string line(rsize, 0);
	while (ifs && ifs.read(&line[0], line.size()))
	{
		ofs.put(done ? ',' : ' ');
		ofs.put('{');
		ofs.put('\n');
		//スケルトンに変換結果を格納
		fdg.conv(line, conv);

		//変換後の項目ごと
		int c = 0;
		for (generic::properties::const_iterator i = conv.begin(), e = conv.end()
			; i != e; ++i)
		{
			utf8.name = "";
			utf8.value = "";

			//UTF-8変換
			utf8.enc.encode(i->name, utf8.name);
			utf8.enc.encode(i->value, utf8.value);

			//JSONエスケープ
			the::jsonescape(&utf8.name[0], &jname[0]);
			the::jsonescape(&utf8.value[0], &jvalue[0]);

			ofs.put(c++ ? ',' : ' ');
			ofs.put('"');
			ofs.write(jname.c_str(), ::strlen(jname.c_str()));
			ofs.put('"');
			ofs.put(':');
			ofs.put('"');
			ofs.write(jvalue.c_str(), ::strlen(jvalue.c_str()));
			ofs.put('"');
			ofs.put('\n');
		}
		//途中経過通知
		if (done % frame == 0)
		{
			notifyboard(done);
		}
		done++;

		ofs.put('}');
		ofs.put('\n');
	}

	ofs << "]" << endl;

	ifs.close();
	ofs.close();
	notifyboard(done);
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