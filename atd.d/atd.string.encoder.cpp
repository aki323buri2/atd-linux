// atd.string.encoder.cpp
#include "atd.h"
using namespace atd;

#include <iconv.h>
#include <errno.h>
//====================================================
//= struct string::encoder::impl
//====================================================
struct string::encoder::impl : public object
{
	iconv_t ic;
	string encto;
	string encfrom;
	impl(const string &encto, const string &encfrom);
	~impl();
	uchar encode_byte(uchar from) const;
	ushort encode_word(ushort from) const;
	string encode(const string &s) const;
	bool encode(const string &from, string &to) const;
};
//====================================================
//= struct string::encoder
//====================================================
string::encoder::encoder(const string &encto, const string &encfrom)
: impl(new struct impl(encto, encfrom))
{
}
string::encoder::~encoder()
{
	delete impl;
}
uchar string::encoder::encode_byte(uchar from) const
{
	return impl->encode_byte(from);
}
ushort string::encoder::encode_word(ushort from) const
{
	return impl->encode_word(from);
}
string string::encoder::encode(const string &from) const 
{
	return impl->encode(from);
}
bool string::encoder::encode(const string &from, string &to) const 
{
	return impl->encode(from, to);
}
//====================================================
//= struct string::encoder::implの実装
//====================================================
string::encoder::impl::impl(const string &encto, const string &encfrom)
: encto(encto)
, encfrom(encfrom)
{
	ic = ::iconv_open(encto.c_str(), encfrom.c_str());
};
string::encoder::impl::~impl()
{
	::iconv_close(ic);
}
uchar string::encoder::impl::encode_byte(uchar from) const
{
	uchar to = 0;
	struct { char *from, *to; } pt = { (char *)&from, (char *)&to };
	struct { size_t from, to; } sz = { sizeof(from), sizeof(to) };
	int r = ::iconv(ic
		, &pt.from, &sz.from
		, &pt.to  , &sz.to  
	);
	int e = errno;
	return r == 0 ? to : 0;
}
ushort string::encoder::impl::encode_word(ushort from) const
{
	ushort to = 0;
	struct { char *from, *to; } pt = { (char *)&from, (char *)&to };
	struct { size_t from, to; } sz = { sizeof(from), sizeof(to) };
	int r = ::iconv(ic
		, &pt.from, &sz.from
		, &pt.to  , &sz.to  
	);
	int e = errno;
	return r == 0 ? to : 0;
}
string string::encoder::impl::encode(const string &from) const
{
	string to;
	encode(from, to);
	return to.c_str();
}
bool string::encoder::impl::encode(const string &from, string &to) const
{
	size_t todo = from.size() * 3;//★
	if (to.size() < todo) to.assign(todo, 0);
	
	struct { char *from, *to; } pt = { (char *)&from[0], &to[0] };
	struct { size_t from, to; } sz = { from.size(), to.size() };

	int r = ::iconv(ic
		, &pt.from, &sz.from
		, &pt.to  , &sz.to  
	);
	int e = errno;
	
	return r == 0;
}