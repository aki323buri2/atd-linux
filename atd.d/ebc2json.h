// ebc2json.h
#ifndef __ebc2json_h__
#define __ebc2json_h__
#include "atd.h"
using namespace atd;
namespace ebc2json {;
struct execute
{
	static void parallel(
		const string &ebc
		, const strings &fdgs
		, const strings &keys
		, const strings &jsons
		, bool looksuffix 
	);
};
}//namespace ebc2json
#endif//__ebc2json_h__