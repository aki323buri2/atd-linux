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
	notifyf("%s start ", app.path.c_str());
} 