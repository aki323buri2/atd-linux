// test.cpp
#include "common.h"
#include "cobol.h"
#include "thread.h"


//####################################################
void test(
	string ebc
	, const strings &fdgs
	, const strings &keys
	, const strings &jsons
	, bool looksuffix
)
{
	path::fileinfo_t info = path::fileinfo(ebc);
	notify("");
	notify(ebc);
	info.demo(notify);

	notify("");
	int rsize = 0;
	bool ok = true;
	struct fdglist : public std::vector<cobol::fdg>
	{
		cobol::fdg &add(const string &path)
		{
			push_back(cobol::fdg());
			cobol::fdg &fdg = back();
			std::ifstream ifs(path.c_str(), std::ios::in);
			fdg.loadcobol(ifs);
			return fdg;
		}
	} fdglist;
	for (strings::const_iterator i = fdgs.begin(), e = fdgs.end()
		; i != e; ++i)
	{
		const string &path = *i;
		cobol::fdg &fdg = fdglist.add(path);

		notifyf(">> %s rsize = %d", path::basename(path).c_str(), fdg.rsize);

		if (false) { }
		else if (!rsize) rsize = fdg.rsize;
		else if (rsize != rsize) { ok = false; }
	}
	if (!ok)
	{
		notify("!!!!　レコード長が揃っていません !!!");
		return;//★
	}

	notify("");
	notifyf(">> rsize = %d", rsize);

	//レコード判別キーと配列添え字のマップ
	//出力ストリームの配列
	std::map<uchar, size_t> kk;
	struct ofss : public std::vector<std::ofstream *>
	{
		~ofss()
		{
			destroy();
		}
		void close()
		{
			for (iterator i = begin(), e = end(); i != e; ++i)
			{
				std::ofstream *ofs = *i;
				ofs->close();
			}
		}
		void destroy()
		{
			for (iterator i = begin(), e = end(); i != e; ++i)
			{
				std::ofstream *ofs = *i;
				ofs->close();
				delete ofs;
			}
		}
	} ofss;
	for (strings::const_iterator b = fdgs.begin(), e = fdgs.end(), i = b
		; i != e; ++i)
	{
		size_t ix = i - b;
		const string &key = keys[ix];
		const string &json = jsons[ix];
		kk[key[0]] = ix;
		ofss.push_back(new std::ofstream());
		std::ofstream *ofs = ofss.back();
		ofs->open(json.c_str(), std::ios::out);
		ofs->close();
		ofs->open(json.c_str(), std::ios::app | std::ios::binary);

	}
	
	//ファイル振り分け
	string::ebcdic ebcdic;
	std::ifstream ifs;
	ifs.open(ebc.c_str(), std::ios::in | std::ios::binary);
	string line(rsize, 0);
	size_t offset = looksuffix ? line.size() - 1 : 0;
	int64 done = 0;
	std::map<uchar, int64> cc;

	//進捗バー
	int frame = 50;
	int64 todo = info.size / rsize;
	int64 step = todo / frame;
	string progress;

	while (ifs && ifs.read(&line[0], line.size()))
	{
		uchar c = line[offset];

		c = ebcdic.ebc2sjis_byte(c);
		cc[c]++;

		size_t ix = kk[c];

		std::ofstream &ofs = *ofss[ix];
		ofs.write(&line[0], line.size());

		if (done % step == 0)
		{
			cout << "#" << flush;
		}
		done++;
	}
	cout << endl;

	ofss.close();

	notifyf(">> done = %10d", done);
	for (std::map<uchar, size_t>::iterator i = kk.begin(), e = kk.end()
		; i != e; ++i)
	{
		uchar c = i->first;
		size_t ix = i->second;
		int64 rows = cc[c];
		string json = jsons[ix];
		int64 size = path::filesize(json);
		notifyf(">> '%c':%-10s %10lld lines / %10lld bytes"
			, c
			, json.c_str()
			, rows
			, size
		);
	}

}
//####################################################
