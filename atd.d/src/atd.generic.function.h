// atd.generic.function.h
#ifndef __atd_generic_function_h__
#define __atd_generic_function_h__
#include "atd.h"
namespace atd {;
namespace generic {;
//====================================================
//= struct atd::generic::function
//====================================================
template <typename R, typename A>
struct function : public object
{
	template <typename T>
	function(R (T::*method)(A), T *offset);
	function(R (*func)(A));
	function();
	R operator ()(A a) const;
	R invoke(A a) const;
	bool operator == (const function &that) const;
	bool operator != (const function &that) const;
	bool operator <  (const function &that) const;
	typedef R(*func_type)(A);
	typedef R(object::*method_type)(A);
	typedef object *offset_type;
protected:
	func_type func;
	method_type method;
	offset_type offset;
};
//====================================================
//= struct atd::generic::function<R, void>
//====================================================
template <typename R>
struct function<R, void> : public object
{
	template <typename T>
	function(R (T::*method)(), T *offset);
	function(R (*func)());
	function();
	R operator ()() const;
	R invoke() const;
	bool operator == (const function &that) const;
	bool operator != (const function &that) const;
	bool operator <  (const function &that) const;
	typedef R(*func_type)();
	typedef R(object::*method_type)();
	typedef object *offset_type;
protected:
	func_type func;
	method_type method;
	offset_type offset;
};
}//namespace generic
}//namespace atd
//====================================================
#include "atd.generic.function.impl.h"
//====================================================
#endif//__atd_generic_function_h__



