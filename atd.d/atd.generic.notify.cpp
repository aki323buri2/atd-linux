// atd.generic.notify.cpp
#include "atd.h"
using namespace atd;
using namespace atd::generic;
//====================================================
//= struct atd::generic::notify
//====================================================
notify::notify(void (*func)(const string &))
: generic::function<void, const string &>(func)
{
}
notify::notify()
: generic::function<void, const string &>()
{
}
//====================================================
//= struct atd::generic::notify
//====================================================
notifyf::notifyf(void (*func)(const string &))
: notify(func)
{
}
notifyf::notifyf()
: notify()
{
}
//()オペレータ
void notifyf::operator ()(const char *format, ...) const
{
	va_list va;

	//必要バイト数
	va_start(va, format);
	string s(::vsnprintf(0, 0, format, va) + 1, 0);
	va_end(va);

	//本番実行（Windowsと違って都度々々vaを初期化する必要あり）
	va_start(va, format);
	::vsnprintf(&s[0], s.size(), format, va);
	va_end(va);

	//notifyクラスのinvoke()を呼び出す
	return notify::invoke(s.c_str());
}
//notifyクラス受け入れコンストラクタ
notifyf::notifyf(const notify &that)
: notify(that)
{
}