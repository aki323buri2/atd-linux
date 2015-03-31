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
	try
	{
		r = frame(argc, argv);
	}
	catch (std::exception &e)
	{
		notifyf("!!!! %s error (%s) : %s"
			, app.basename.c_str()
			, datetime::now().strftime().c_str()
			, e.what()
		);
	}
	return r;
}
int frame(int argc, char **argv)
{
	notifyf(">>>> %s start (%s) >>>>", app.filename.c_str(), datetime().now().strftime().c_str());
	int r = run(argc, argv);
	notifyf(">>>> %s e n d (%s) >>>>", app.filename.c_str(), datetime().now().strftime().c_str());
	notify ("");
	return r;
}
int run(int argc, char **argv)
{
	cout << "*> app.path    : " << app.path << endl;
	cout << "*> app.dirname : " << app.dirname << endl;
	cout << "*> app.logfile : " << app.logfile << endl;
	return 0;
} 