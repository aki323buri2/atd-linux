// cobol.cpp
#include "cobol.h"
//====================================================
//= struct cobol
//====================================================
//初期化実行
struct regex cobol::ffd::re;
struct string::ebcdic cobol::ffd::ebcdic;
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
, subdn(1)//★
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
void cobol::fdg::clear()
{
	rsize = 0;
	names.clear();
	std::vector<ffd>::clear();
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
	//OCCURS展開
	pre.expandto(*this);
}
//====================================================
//= OCCURS展開
//====================================================
void cobol::fdg::expandto(fdg &that) const
{
	expandto(that, begin(), 0);
}
cobol::fdg::const_iterator cobol::fdg::expandto(
	  fdg &that
	, const_iterator where
	, int sub
	, int subdn//分母
) const
{
	//レコード長累積
	int &rsize		= that.rsize;
	cout << rsize << endl;
	//止め
	if (where == end()) return where;

	//現在のイレレータの内容をコピー
	struct ffd ffd = *where;//コピー
	int  occurs		= ffd.occurs;
	int &real		= ffd.real	;
	int &offset		= ffd.offset;
	string &type	= ffd.type	;

	//繰り返しのインデックス
	ffd.sub = sub;
	ffd.subdn = subdn;

	//イテレータコピー
	const_iterator cursor = where;

	//OCCURS分繰り返す！
	if (!occurs) occurs = 1;
	for (int i = 0; i < occurs; i++)
	{
		if (!type.length())
		{
			//----------------------------------------
			//集団項目である
			//----------------------------------------
			//イテレータをスタート位置にリセット
			cursor = where;

			//LV番号が大きい間再帰呼び出す
			++cursor;
			int lv = ffd.lv;
			while (cursor != end() && cursor->lv > lv)
			{
				cursor = expandto(that, cursor, i, occurs);
			}
		}
		else 
		{
			//----------------------------------------
			//集団項目でない
			//----------------------------------------
			//項目を追加
			offset = rsize;
			rsize += real;

			//名前の重複チェック
			std::map<string, int> &names = that.names;
			string &name = ffd.name;
			string &id = ffd.id;
			int &count = names[name];
			count++;
			//重複していた場合 id = "[名前]_[インクリメント番号]"
			id = 
				count > 1 
				? string::format("%s_%d", name.c_str(), count)
				: name 
				;
			that.push_back(ffd);
			//イテレータを進める
			++cursor;
		}
	}
	return cursor;
}
//====================================================
//= プロパティリストのスケルトン作成
//====================================================
generic::properties cobol::fdg::propskelton() const 
{
	generic::properties prop;

	//名前重複回避用
	std::map<string, int> map;

	for (const_iterator i = begin(), e = end()
		; i != e; ++i)
	{
		const ffd &ffd = *i;

		const string &id   = ffd.id;
		const string &type = ffd.type;
		const bool pack = ffd.pack;
		const int real = ffd.real;
		const int left = ffd.left;
		const int right = ffd.right;

		int size = pack ? (left + right) : real;
		
		//要素を追加して実バイトサイズ分の領域確保
		string &value = prop.value_of(id);
		value.assign(size, type[0]);
	}

	return prop;
}
//====================================================
//= 変換
//====================================================
void cobol::fdg::conv(const string &line, generic::properties &conv) const
{
	const char *ptr = &line[0];

	for (const_iterator i = begin(), e = end()
		; i != e; ++i)
	{
		const ffd &ffd = *i;
		const string &id = ffd.id;
		string &value = conv.value_of(id);

		//変換
		ffd.conv(ptr, &value[0]);
	}
}
void cobol::ffd::conv(const char *src, char *ptr) const 
{
	uchar *p = (uchar *)(src + offset);

	bool kanji = (type == "N");

	if (kanji)
	{
		//--------------------------------------------
		//- JEF漢字
		//--------------------------------------------
		for (int i = 0; i < real; i++)
		{
			uchar hi = *(p++);
			uchar lo = *(p++);
			ushort jef  = hi << 8 | lo;
			ushort sjis = ebcdic.jef2sjis_word(jef);
			*(ptr++) = sjis >> 8 & 0xff;
			*(ptr++) = sjis & 0xff;
			i++;
		}
	}
	else if (pack)
	{
		//--------------------------------------------
		//-　パック項目
		//--------------------------------------------
		for (int i = 0; i < real; i++)
		{
			::snprintf(ptr, 3, "%02x", *(p++));
			ptr += 2;
		}
	}
	else
	{
		//--------------------------------------------
		//- 半角英数（PIC 9 or X）
		//--------------------------------------------
		for (int i = 0; i < real; i++)
		{
			*(ptr++) = ebcdic.ebc2sjis_byte(*(p++));
		}
	}

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
		" %7s"
		"%-25s"
		"%-10s"
		, lv
		, name.c_str()
		, (subdn > 1 ? string::format("(%2d/%2d)", sub+1, subdn).c_str() : "")
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
