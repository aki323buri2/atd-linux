//atd.object.h
#ifndef __atd_object_h__
#define __atd_object_h__
#include "atd.h"
namespace atd {;;
//====================================================
//= struct atd::object
//====================================================
struct object 
{
	virtual ~object() { }
};
//====================================================
//= struct atd::objects
//====================================================
struct objects : public object, public std::vector<object *>
{
	objects();
	objects(object *o0)																												{ entry(o0); }
	objects(object *o0, object *o1)																									{ entry(o0, o1); }
	objects(object *o0, object *o1, object *o2)																						{ entry(o0, o1, o2); }
	objects(object *o0, object *o1, object *o2, object *o3)																			{ entry(o0, o1, o2, o3); }
	objects(object *o0, object *o1, object *o2, object *o3, object *o4)																{ entry(o0, o1, o2, o3, o4); }
	objects(object *o0, object *o1, object *o2, object *o3, object *o4, object *o5)													{ entry(o0, o1, o2, o3, o4, o5); }
	objects(object *o0, object *o1, object *o2, object *o3, object *o4, object *o5, object *o6)										{ entry(o0, o1, o2, o3, o4, o5, o6); }
	objects(object *o0, object *o1, object *o2, object *o3, object *o4, object *o5, object *o6, object *o7)							{ entry(o0, o1, o2, o3, o4, o5, o6, o7); }
	objects(object *o0, object *o1, object *o2, object *o3, object *o4, object *o5, object *o6, object *o7, object *o8)				{ entry(o0, o1, o2, o3, o4, o5, o6, o7, o8); }
	objects(object *o0, object *o1, object *o2, object *o3, object *o4, object *o5, object *o6, object *o7, object *o8, object *o9)	{ entry(o0, o1, o2, o3, o4, o5, o6, o7, o8, o9); }

	objects &entry(object *o0)																												{ return entryf( 1, o0); }
	objects &entry(object *o0, object *o1)																									{ return entryf( 2, o0, o1); }
	objects &entry(object *o0, object *o1, object *o2)																						{ return entryf( 3, o0, o1, o2); }
	objects &entry(object *o0, object *o1, object *o2, object *o3)																			{ return entryf( 4, o0, o1, o2, o3); }
	objects &entry(object *o0, object *o1, object *o2, object *o3, object *o4)																{ return entryf( 5, o0, o1, o2, o3, o4); }
	objects &entry(object *o0, object *o1, object *o2, object *o3, object *o4, object *o5)													{ return entryf( 6, o0, o1, o2, o3, o4, o5); }
	objects &entry(object *o0, object *o1, object *o2, object *o3, object *o4, object *o5, object *o6)										{ return entryf( 7, o0, o1, o2, o3, o4, o5, o6); }
	objects &entry(object *o0, object *o1, object *o2, object *o3, object *o4, object *o5, object *o6, object *o7)							{ return entryf( 8, o0, o1, o2, o3, o4, o5, o6, o7); }
	objects &entry(object *o0, object *o1, object *o2, object *o3, object *o4, object *o5, object *o6, object *o7, object *o8)				{ return entryf( 9, o0, o1, o2, o3, o4, o5, o6, o7, o8); }
	objects &entry(object *o0, object *o1, object *o2, object *o3, object *o4, object *o5, object *o6, object *o7, object *o8, object *o9)	{ return entryf(10, o0, o1, o2, o3, o4, o5, o6, o7, o8, o9); }

	objects &entryf(int count, ...);
	void destroy();
};
//====================================================
//= struct atd::managed::objects
//====================================================
namespace managed {;
struct objects : public atd::objects
{
	objects()																														: atd::objects() { }
	objects(object *o0)																												: atd::objects(o0) { }
	objects(object *o0, object *o1)																									: atd::objects(o0, o1) { }
	objects(object *o0, object *o1, object *o2)																						: atd::objects(o0, o1, o2) { }
	objects(object *o0, object *o1, object *o2, object *o3)																			: atd::objects(o0, o1, o2, o3) { }
	objects(object *o0, object *o1, object *o2, object *o3, object *o4)																: atd::objects(o0, o1, o2, o3, o4) { }
	objects(object *o0, object *o1, object *o2, object *o3, object *o4, object *o5)													: atd::objects(o0, o1, o2, o3, o4, o5) { }
	objects(object *o0, object *o1, object *o2, object *o3, object *o4, object *o5, object *o6)										: atd::objects(o0, o1, o2, o3, o4, o5, o6) { }
	objects(object *o0, object *o1, object *o2, object *o3, object *o4, object *o5, object *o6, object *o7)							: atd::objects(o0, o1, o2, o3, o4, o5, o6, o7) { }
	objects(object *o0, object *o1, object *o2, object *o3, object *o4, object *o5, object *o6, object *o7, object *o8)				: atd::objects(o0, o1, o2, o3, o4, o5, o6, o7, o8) { }
	objects(object *o0, object *o1, object *o2, object *o3, object *o4, object *o5, object *o6, object *o7, object *o8, object *o9)	: atd::objects(o0, o1, o2, o3, o4, o5, o6, o7, o8, o9) { }

	~objects();
};
}//namespace managed
}//namespace atd
#endif//__atd_object_h__
