// ebc2json.job.cpp
#include "common.h"
#include "ebc2json.job.h"
using namespace ebc2json;
job::job(
	  const string &ebc
	, const string &fdg
	, const string &json
)
: todo(0), done(0)
{
	path.ebc = ebc;
	path.fdg = fdg;
	path.json = json;
	this->fdg.loadcobol(path.fdg);
}
job::~job()
{
}
job::map::map()
{
}
job::map::~map()
{
}
void job::map::clear()
{
}
void job::map::init(const strings &fdgs, const strings &keys, const strings &jsons)
{
}
void job::map::read(const string &ebc, bool looksuffix)
{
}