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
};
//====================================================
//= struct cobol::ffd
//====================================================
struct cobol::ffd : public object
{
	int		lv		;
	string	name	;
	bool	sig		;
	string	type	;
	int		left	;
	int		right	;
	bool	pack	;
	int		occurs	;

	int		offset	;
	int		real	;

	regex re;

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

	string demo() const;
};
//====================================================
//= struct cobol::fdg
//====================================================
struct cobol::fdg : public object, public std::vector<ffd>
{
	int rsize;

	fdg();

	//COBOLイメージのロード
	void loadcobol(std::istream &is, const string &encfrom = "SJIS-WIN");
};

#endif//__cobol_h__



