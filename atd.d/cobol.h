// cobol.h
#ifndef __cobol_h__
#define __cobol_h__
#include "atd.h"
using namespace atd;
//====================================================
//= struct cobol
//====================================================
struct cobol : public object
{
	struct ffd;
	struct fdg;

	//イニシャライザ
	struct init
	{
		init();
	};
	static init init;
};
//====================================================
//= struct cobol::ffd
//====================================================
struct cobol::ffd : public object
{
	static regex re;
	static string::ebcdic ebcdic;

	int		lv		;//LV番号
	string	name	;//名前
	bool	sig		;//符号
	string	type	;//データタイプ
	int		left	;//整数部桁数
	int		right	;//小数部桁数
	bool	pack	;//パック項目
	int		occurs	;//OCCURS（繰り返し数）

	int		offset	;//開始バイト位置
	int		real	;//実バイト数
	int		sub		;//OCCURS繰り返しインデックス
	int		subdn	;//OCCURS繰り返しインデックスの分母(dn: dinominatorの略）
	string	id		;//重複しない名前


	ffd(
		  int		lv		= 0
		, string	name	= ""
		, bool		sig		= false
		, string	type	= ""
		, int		left	= 0
		, int		right	= 0
		, bool		pack	= false
		, int		occurs	= 0
	);
	bool parsecobol(const string &line);

	//変換処理
	void conv(const char *src, int srclen, char *ptr, int size) const;


	string demo() const;
};
//====================================================
//= struct cobol::fdg
//====================================================
struct cobol::fdg : public object, public std::vector<ffd>
{
	int rsize;//レコード長

	std::map<string, int> names;//重複チェック用

	fdg();
	void clear();

	//COBOLイメージのロード
	void loadcobol(std::istream &is, const string &encfrom = "SJIS-WIN");

	//OCCURS展開
	void expandto(fdg &that) const;
	const_iterator expandto(fdg &that, const_iterator where, int sub, int subdn = 1) const;

	//プロパティリストのスケルトン作成
	generic::properties propskelton() const;

	//変換
	void conv(const string &line, generic::properties &conv) const;

	void demo(const generic::notify &notify) const;
};

#endif//__cobol_h__



