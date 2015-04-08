// cobol.cpp
#include "cobol.h"
//====================================================
//= struct cobol
//====================================================
//初期化実行
struct regex cobol::ffd::re;
struct cobol::init cobol::init;
cobol::init::init()
{
	//正規表現コンパイル
	ffd::re.compile(
		"^([0-9])?"							//DUETで作ったファイルには行番号が付いてしまう
		"\\s*([0-9]{2})"					//LV番号
		"\\s+([^ ]+)"						//識別名
		"("
			"\\s+PIC\\s+"
			"(S?)"							//符号付き？
			"(9|X|N)"						//データタイプ
			"\\(([0-9]+)\\)"				//整数部桁数
			"(V9\\(([0-9]+)\\)|V(9+))?"		//小数部桁数
		")?"
		"(\\s+(PACKED-DECIMAL|COMP-3))?"	//パック項目？
		"(\\s+OCCURS\\s+([0-9]+))?"			//OCCURS
	);
}
//====================================================
//= struct cobol::ffd
//====================================================
cobol::ffd::ffd(
	  int		lv		//= 0
	, string	name	//= ""
	, bool		sig		//= false
	, string	type	//= ""
	, int		left	//= 0
	, int		right	//= 0
	, bool		pack	//= false
	, int		occurs	//= 0
)
: lv(lv)
, name(name)
, sig(sig)
, type(type)
, left(left)
, right(right)
, pack(pack)
, occurs(occurs)
, offset(0)
, real(0)
, sub(0)
{
}
bool cobol::ffd::parsecobol(const string &line)
{
	//正規表現マッチ
	strings match = re.match(line);

	//マッチしなければ解析しない
	if (match.size() <= 1) return false;

	lv		= match[2].toint();
	name	= match[3] ;
	sig		= match[5].length();
	type	= match[6];
	left	= match[7].toint();
	right	= 0;//★
	pack	= match[11].length();
	occurs	= match[14].toint();

	//文末ピリオド除去
	name = regex::replace("\\..*", name, "");

	//------------------------------------------------
	//小数部桁数解析
	string s = match[8];//"V9(2)" or "V99"
	string n = match[9];//"2" or "99"
	if (s.find('(') == string::npos)
	{
		//小数部記述に'('があれば V9(2)
		right = n.toint();
	}
	else
	{
		//小数部記述に'('が無ければ V99
		right = n.length();
	}
	//------------------------------------------------
	//リアルサイズ計算
	real = left + right;
	if (type == "N")
	{
		//全角
		real *= 2;
	}
	if (pack)
	{
		//パック項目
		real = (real/2) + 1	;
	}
	//------------------------------------------------

	//解析したよ！
	return true;
}
//====================================================
//= struct cobol::fdg
//====================================================
cobol::fdg::fdg()
: rsize(0)
{
}
//入力ストリームからロード
void cobol::fdg::loadcobol(std::istream &is, const string &encfrom)
{
	//文字コードエンコーダ
	string encto = "UTF-8";
	string::encoder encoder(encto, encfrom);

	//プレ作成
	fdg pre;
	string line;
	while (is && std::getline(is, line))
	{
		//文字コード変換
		line = encoder.encode(line);

		//１行解析
		ffd ffd;
		if (!ffd.parsecobol(line)) continue;

		pre.push_back(ffd);
	}

	pre.expandto(*this);
}
//OCCURS / 集団項目展開
void cobol::fdg::expandto(fdg &that) const
{
	expandto(that, begin(), 0);
}
cobol::fdg::const_iterator cobol::fdg::expandto(
	  fdg &that
	, const_iterator where
	, int sub
) const
{
	struct ffd ffd = *where;//コピー

	int &rsize		= that.rsize;
	int  occurs		= ffd.occurs;
	int &real		= ffd.real	;
	int &offset		= ffd.offset;
	string &name	= ffd.name	;
	string &type	= ffd.type	;

	ffd.sub = sub;

	if (!occurs) occurs = 1;
	
	const_iterator cursor = where;

	for (int i = 0; i < occurs; i++)
	{
		if (!type.length())
		{
			//集団項目
			int lv = ffd.lv;

			cursor = where;
			++cursor;
			while (cursor != that.end() && cursor->lv > lv)
			{
				cursor = expandto(that, cursor, i);
			}
		}
		else 
		{
			offset = rsize;
			rsize += real;
			that.push_back(ffd);
			cursor++;
		}
	}
	return cursor;

}
//====================================================
//= デモ表示
//====================================================
void cobol::fdg::demo(const generic::notify &notify) const
{
	for (const_iterator i = begin(), e = end(); i != e; ++i)
	{
		notify(i->demo());
	}
}
string cobol::ffd::demo() const 
{
	return string::format(
		"%02d"
		" %-10s"
		" %4s"
		"%-25s"
		"%-10s"
		, lv
		, name.c_str()
		, (sub ? string::format("(%2d)", sub+1).c_str() : "")
		, (!type.length() 
			? "" 
			: string::format(
				" %1s%1s"
				"(%2d)"
				"%-5s"
				" %-1s" 
				" (%3d-%2d)"
				, (sig ? "S" : ""), type.c_str()
				, left
				, (right ? string::format("V(%02d)", right).c_str() : "")
				, (pack ? "P" : "")
				, offset, real
				).c_str()
			)
		, (!occurs 
			? ""
			: string::format(
				"OCCURS %3d"
				, occurs
				).c_str()
			)
	);
}
