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
	string subject = "/hoge/ﾌｶﾞ表/";
	string pattern = "/ﾌｶﾞ";
	regex re;
	re.compile(pattern);
	strings match = re.match(subject);

	for (strings::iterator i = match.begin(), e = match.end()
		; i != e; ++i)
	{
		cout << "match[" << (i - match.begin()) << "] : " << *i << endl;
	}
} 