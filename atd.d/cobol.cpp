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
		"^([0-9]{6})?"							//DUETで作ったファイルには行番号が付いてしまう
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
, subdn(1)//★OCCURSインデックスの分母
{
}
//====================================================
//= COBOLイメージ記述の解析
//====================================================
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
	if (s.find('(') != string::npos)
	{
		//小数部記述に'('があれば V9(2)
		string n = match[9];//"2"
		right = n.toint();
	}
	else if (s.length())
	{
		//小数部記述に'('が無ければ V99
		string n = match[10];//"99";
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
//====================================================
//= 入力ストリームからロード
//====================================================
void cobol::fdg::loadcobol(const string &cobol, const string &encfrom)
{
	struct smart
	{
		std::istream *p;
		smart() : p(0) { }
		~smart() { delete p; }
	} smart;

	if (path::isfile(cobol))
	{
		smart.p = new std::ifstream(cobol.c_str(), std::ios::in);
	}
	else
	{
		smart.p = new std::stringstream(cobol.c_str());
	}
	loadcobol(*smart.p, encfrom);
}
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
		const bool sig = ffd.sig;
		const int real = ffd.real;
		const int left = ffd.left;
		const int right = ffd.right;

		//出力にバイト数計算
		int size = left + right;
		if (type == "N") size = real;
		if (right) size++;//小数点用
		if (sig  ) size++;//符号用
		
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
void cobol::ffd::conv(const char *ebcline, char *ptr) const 
{
	//変換先ポインタメモっとく
	char *memo = ptr;

	//変換元のポインタをオフセット移動
	uchar *ebc = (uchar *)(ebcline + offset);


	bool kanji = (type == "N");
	bool minus = false;

	if (kanji)
	{
		//--------------------------------------------
		//- JEF漢字
		//--------------------------------------------
		for (int i = 0; i < real; i++)
		{
			//WORD単位の変換
			uchar hi = *(ebc++);
			uchar lo = *(ebc++);
			ushort jef  = hi << 8 | lo;
			ushort sjis = ebcdic.jef2sjis_word(jef);
			*(ptr++) = sjis >> 8 & 0xff;
			*(ptr++) = sjis & 0xff;
			i++;//２バイトずつ
		}
	}
	else if (pack)
	{
		//--------------------------------------------
		//-　パック項目
		//--------------------------------------------
		bool even = (left + right) % 2 == 0;
		for (int i = 0; i < real; i++)
		{
			//WORD単位で16進表記
			uchar c = *(ebc++);
			uchar buf[3] = {0};
			::snprintf((char *)buf, sizeof(buf), "%02x", c);
			uchar hi = buf[0];
			uchar lo = buf[1];

			if (i == 0 && even)
			{
				//バイト数が偶数の場合先頭の１バイトは捨てる
				*(ptr++) = lo;
			}
			else if (i < real - 1)
			{
				*(ptr++) = hi;
				*(ptr++) = lo;
			}
			else if (i == real - 1)
			{
				*(ptr++) = hi;
				//最後の１バイトは符号判定
				minus = (lo == 'd');
			}
		}
	}
	else
	{
		//--------------------------------------------
		//- 半角英数（PIC 9 or X）
		//--------------------------------------------
		for (int i = 0; i < real; i++)
		{
			//BYTE単位にEBCDIC>>SJIS変換
			*(ptr++) = ebcdic.ebc2sjis_byte(*(ebc++));
		}
		if (sig)
		{
			//サインド項目
			ptr--;//１つ戻る（末尾を指す）
			uchar c = (uchar)*ptr;

			// '}' : +0  '{' : -0
			// 'a' : +1  'j' : -1
			// 'b' : +2  'k' : -2
			// 'c' : +3  'l' : -3
			//    ...       ...  
			// 'i' : +9  'r' : -9

			int n = 0;
			if (false) { }
			else if (c == '{') { n = 0; minus = false; }
			else if (c == '}') { n = 0; minus = true ; }
			else if (BETWEEN('a', c, 'i')) { n = c - 'a'; minus = false; }
			else if (BETWEEN('j', c, 'r')) { n = c - 'j'; minus = true ; }
			
			//末尾の文字（数字）をセット
			*ptr = '0' + n;

		}
	}

	//------------------------------------------------
	//- 最終調整します
	//------------------------------------------------
	//変換先ポインタ戻す
	ptr = memo;

	if (sig)
	{
		//--------------------------------------------
		//- サインド項目
		//--------------------------------------------
		//１文字ずつ後ろにずらす
		int size = left + right;
		char *p = ptr + size;
		for (int i = 0; i < size; i++)
		{
			p--;
			*(p + 1) = *p;
		}
		//先頭に'+/-'を付ける
		*ptr = minus ? '-' : '+';
	}
	if (right)
	{
		//--------------------------------------------
		//- 小数点以下
		//--------------------------------------------
		//小数点を挿入する位置をポイント
		char *p = ptr + left;
		if (sig) p++;
		//小数点を入れる位置を記憶
		char *point = p;
		//小数点部分を１文字ずつ後ろにずらす
		for (int i = 0; i < right; i++)
		{
			p++;
			*(p + 1) = *p;
		}
		//小数点を挿入
		*(point) = '.';

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
