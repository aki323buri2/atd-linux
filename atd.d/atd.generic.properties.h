// atd.generic.properties.h
#ifndef __atd_generic_properties_h__
#define __atd_generic_properties_h__
#include "atd.h"
#include <list>
namespace atd {;
namespace generic {;
struct property;
struct properties;
//====================================================
//= struct atd::generic::property
//====================================================
struct property : public object
{
	string name;
	string value;
	property(const string &name, const string &value);

	static bool isnull(const string &value);
	string demo(int nlen = 15) const;

	//subclasses:
	struct primitive;
	struct hash;
};
//====================================================
//= struct atd::generic::property::primitive
//====================================================
struct property::primitive 
{
	const char *name;
	const char *value;
};
//====================================================
//= struct atd::generic::property::hash
//====================================================
struct property::hash
: public object, public std::map<string, std::list<property>::iterator>
{
	typedef std::list<property> owner_type;
	owner_type *owner;

	hash(owner_type *owner);

	owner_type::iterator iterator_of(const string &name);
	owner_type::const_iterator iterator_at(const string &name) const;
};
//====================================================
//= struct atd::generic::properties
//====================================================
struct properties : public object, public property::hash::owner_type 
{
	typedef property::hash::owner_type base;

	property::hash hash;
	properties();
	void clear();

	bool has_key(const string &name) const;

	//キーによる参照（無ければ追加）
	string &operator [](const string &name);
	string &value_of(const string &name);

	//キーによるconst参照（無ければ代替値）
	string at(const string &name, const string &backup = "") const;

	//デモ
	void demo(const generic::notify &notify, int nlen = 15) const;

	//JSONパース
	void json_decode(std::istream &is);
	void json_encode(std::ostream &os) const;
	void json_decode(const string &json);
	string json_encode() const;

	//原始構造体をロード
	void load_primitive(struct property::primitive *primitive);

	//マージ
	void supply  (const properties &with);//隙間を埋める
	void overwrap(const properties &with);//上書きする
};
}//namespace generic
}//namespace atd
#endif//__atd_generic_properties_h__



