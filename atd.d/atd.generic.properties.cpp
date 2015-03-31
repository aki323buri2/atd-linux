// atd.generic.properties.h
#include "atd.h"
using namespace atd;
using namespace atd::generic;
// special thanks to https://github.com/kazuho/picojson
#include "picojson.h"
//====================================================
//= struct atd::generic::property
//====================================================
property::property(
	  const string &name
	, const string &value
)
: name(name)
, value(value)
{
}
bool property::isnull(const string &value)
{
	return value == "" || value == "null";
}
string property::demo(int nlen) const
{
	return string::format(
		"%-*s: %s"
		, nlen
		, name .c_str()
		, value.c_str()
	);
}
//====================================================
//= struct atd::generic::property::hash
//====================================================
property::hash::hash(owner_type *owner)
: owner(owner)
{
}
property::hash::owner_type::iterator
property::hash::iterator_of(const string &name)
{
	//キー検索
	iterator i = find(name);
	if (i == end())
	{
		//見つからなければ、親listに要素を追加してそのイテレータを追加
		owner_type::iterator add = owner->insert(owner->end(), property(name, ""));
		insert(value_type(name, add));
		return add;
	}
	else 
	{
		//見つかった
		return i->second;
	}
}
property::hash::owner_type::const_iterator
property::hash::iterator_at(const string &name) const
{
	const_iterator i = find(name);
	return i == end() ? owner->end() : i->second;
}
//====================================================
//= struct atd::generic::properties
//====================================================
properties::properties()
: hash(this)
{
}
void properties::clear()
{
	hash.clear();
	base::clear();
}
bool properties::has_key(const string &name) const
{
	return hash.find(name) != hash.end();
}
string &properties::operator [](const string &name)
{
	return value_of(name);
}
string &properties::value_of(const string &name)
{
	return hash.iterator_of(name)->value;
}
string properties::at(const string &name, const string &backup) const
{
	const_iterator i = hash.iterator_at(name);
	return i == end() ? backup : i->value;
}
//demo
void properties::demo(const generic::notify &notify, int nlen) const
{
	for (const_iterator i = begin(), e = end(); i != e; ++i)
	{
		notify(i->demo(nlen));
	}
}
//====================================================
//= json parse
//====================================================
void properties::json_decode(std::istream &is)
{
	picojson::value v(picojson::object_type, false);
	is >> v;

	if (!v.is<picojson::object>()) return;

	picojson::object &o = v.get<picojson::object>();
	for (picojson::object::iterator i = o.begin(), e = o.end()
		; i != e; ++i)
	{
		value_of(i->first) = i->second.to_str();
	}
}
void properties::json_encode(std::ostream &os) const 
{
	picojson::value v(picojson::object_type, false);
	picojson::object &o = v.get<picojson::object>();

	for (const_iterator i = begin(), e = end(); i != e; ++i)
	{
		//★Win版と違ってエラーになるので強引に！！
	//	o[i->name] = i->value;
		o[i->name] = picojson::value(i->value);
	}

	//★Win版と違ってエラーになるので強引に！！
//	os << o;
	v.serialize(std::ostream_iterator<char>(os));
}
void properties::json_decode(const string &json)
{
	if (path::exists(json, F_OK | R_OK))
	{
		std::ifstream ifs(json.c_str());
		json_decode(ifs);
	}
	else
	{
		std::istringstream iss(json);
		json_decode(iss);
	}
}
string properties::json_encode() const 
{
	std::ostringstream oss;
	json_encode(oss);
	return oss.str();
}