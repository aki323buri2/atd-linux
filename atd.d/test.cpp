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
		//EBCDICファイルクリア
		j->ebcclear();
		j->ebcopenappend();
	}
}
//####################################################
