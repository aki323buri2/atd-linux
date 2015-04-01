// test.cpp
#include "common.h"

void sleep(int milliseconds)
{
	int base = 1000; 
	timespec ts = { milliseconds / base, milliseconds % base };
	::nanosleep(&ts, 0); 
}

int main(int argc, char **argv)
{
	int r = 0;
	string err;
	try
	{
		r = frame(argc, argv);
	}
	catch (std::exception &e)
	{
		notifyf("!!!! %s error (%s) : %s"
			, app.basename.c_str()
			, datetime::now().strftime().c_str()
			, err.c_str()
		);
		return 1; 
	}
	return r;
}
int frame(int argc, char **argv)
{
	string name = app.filename;
	notifyf(">>>> %s start (%s) >>>>", name.c_str(), datetime::now().strftime().c_str());
	int r = run(argc, argv);
	notifyf(">>>> %s e n d (%s) >>>>", name.c_str(), datetime::now().strftime().c_str());
	notify ("");
	return r;
}
int run(int argc, char **argv) 
{
	generic::properties pp;
	generic::property::primitive pri[20] = {
		{ "hoge1", "fuga1" }, 
		{ "hoge2", "fuga2" }, 
		{ "hoge3", "fuga3" }, 
		{ "hoge4", "fuga4" }, 
	};
	pp.load_primitive(&pri[0]);

	pp.json_decode(
		"{"
		"  \"jhoge1\": \"jfuga1\""
		", \"jhoge2\": \"jfuga2\""
		", \"jhoge3\": \"jfuga3\""
		", \"jhoge4\": \"jfuga4\""
		"}"
	);

	pp.demo(notify);

	return 0;
} 