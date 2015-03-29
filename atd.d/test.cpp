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
	cout << datetime::now().strftime() << endl;

	datetime d;
	d.mon = 4;
	d.day = 0;
	d.mktime();
	cout << d.yobi() << endl;

}