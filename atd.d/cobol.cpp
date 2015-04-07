// cobol.cpp
#include "cobol.h"
//====================================================
//= struct cobol
//====================================================
struct cobol::init : public object
{
	init();
};
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
		"(\\s+(OCCURS\\s+([0-9]+)))?"		//OCCURS
	);
}
//初期化実行
regex cobol::ffd::re;
namespace { static cobol::init init; };
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
	//解析したよ！
	return true;
}
string cobol::ffd::demo() const 
{
	std::ostringstream oss;
	oss << string::format(
			"%02d %-10s %1s%1s %-10s %-10s %-10s"
			, lv, name.c_str(), type.c_str()
			, sig ? "S" : ""
			, left ? string::format("(%3d-%d)", left, right).c_str() : ""
			, occurs ? string::format("OCCURS %2d", occurs).c_str() : ""
			, pack ? "PACKED" : ""
			);
	return oss.str();
}
//====================================================
//= struct cobol::fdg
//====================================================
cobol::fdg::fdg()
: rsize(0)
{
}
void cobol::fdg::loadcobol(std::istream &is, const string &encfrom)
{
	string encto = "UTF-8";
	string::encoder *encoder = 
		encto == encfrom ? 0 : new string::encoder(encfrom, encto)
		;
	managed::objects garbase(encoder);

	string line;
	while (is && std::getline(is, line))
	{
		line = encoder ? encoder->encode(line) : line;

		ffd ffd;
		if (!ffd.parsecobol(line)) continue;

		push_back(ffd);
	}
}
void cobol::fdg::demo(const generic::notify &notify) const
{
	for (const_iterator i = begin(), e = end(); i != e; ++i)
	{
		notify(i->demo());
	}
}
