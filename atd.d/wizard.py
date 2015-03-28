# wizard.py
import os 
import sys
release = True
release = False
#=====================================================
def make(filename, s):
	if release and os.path.isfile(filename):
		return
	with open(filename, "w", encoding = "utf-8") as f:
		f.write("""\
//{filename}
{s}
""".format(filename = filename, s = s))

#=====================================================
def makeheader(filename, s):
	make(filename, """\
#ifndef {guard}
#define {guard}
{s}
#endif//{guard}\
""".format(guard = guard(filename), s = s))


#=====================================================
def makecpp(filename, s):
	make(filename, """\
{s}\
""".format(guard = guard(filename), s = s))

#=====================================================
def guard(filename):
	return "__%s__" % (filename.replace(".", "_"))

#=====================================================
#=====================================================
ns = "atd"
names = [
	"common", 
	"macro", 
	"path", 
	"datetime", 
]
nocpp = [
	"common", 
	"macro"
]

#=====================================================
#= run()
#=====================================================
def run():
	for name in names:
		headername = "%s.%s.h" % (ns, name)
		cppname = None if name in nocpp else "%s.%s.cpp" % (ns, name)

		#---------------------------------------------
		makeheader("%s.h" %(ns), """\
{s}\
""".format(
			s = "\n".join(["#include \"%s.%s.h\"" %(ns, n) for n in names])
		))
		#---------------------------------------------

		#---------------------------------------------
		makeheader(headername, """\
#include "{ns}.h"
""".format(ns = ns
		))
		#---------------------------------------------
		
		if cppname == None:
			continue
		
		#---------------------------------------------
		makecpp(cppname, "")
		#---------------------------------------------

#=====================================================
#= entry point
#=====================================================
if __name__ == "__main__":
	run()	
