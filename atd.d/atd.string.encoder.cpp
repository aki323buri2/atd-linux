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
	string to;
	string from;
	impl(const string &to, const string &from);
	~impl();
};
string::encoder::impl::impl(const string &to, const string &from)
: to(to)
, from(from)
{
	ic = ::iconv_open(to.c_str(), from.c_str());
};
string::encoder::impl::~impl()
{
	::iconv_close(ic);
}
//====================================================
//= struct string::encoder
//====================================================
string::encoder::encoder(const string &to, const string &from)
: impl(new struct impl(to, from))
{
}
string::encoder::~encoder()
{
	delete impl;
}