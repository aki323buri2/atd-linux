// atd.generic.notify.h
#ifndef __atd_generic_notify_h__
#define __atd_generic_notify_h__
#include "atd.h"
namespace atd {;
namespace generic {;
//====================================================
//= struct atd::generic::notify
//====================================================
struct notify : public function<void, const string &>
{
	template <typename T>
	notify(void (T::*method)(const string &), T *offset);
	notify(void (*func)(const string &));
	notify();
};
template <typename T>
notify::notify(void (T::*method)(const string &), T *offset)
: generic::function<void, const string &>(method, offset)
{
}
//====================================================
//= struct atd::generic::notify
//====================================================
struct notifyf : public notify
{
	template <typename T>
	notifyf(void (T::*method)(const string &), T *offset);
	notifyf(void (*func)(const string &));
	notifyf();

	//()オペレータとnotify受け入れコンストラクタ
	void operator ()(const char *format, ...) const;
	notifyf(const notify &that);
};
template <typename T>
notifyf::notifyf(void (T::*method)(const string &), T *offset)
: notify(method, offset)
{
}
}//namespace generic
}//namespace atd
#endif//__atd_generic_notify_h__




