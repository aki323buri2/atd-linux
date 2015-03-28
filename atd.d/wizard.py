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
	"macro", 
	"common", 
	"object", 
	"string", 
	"path", 
	"datetime", 
]
nocpp = [
	"macro"
	"common", 
]
lock = [
	"macro", 
	"common", 
	"object", 
	"string", 
]

#=====================================================
#= run()
#=====================================================
def run():
	
	#---------------------------------------------
	makeheader("%s.h" %(ns), """\
{s}\
""".format(
		s = "\n".join(["#include \"%s.%s.h\"" %(ns, n) for n in names])
	))
	#---------------------------------------------

	for name in names:

		if name in lock:
			continue

		headername = "%s.%s.h" % (ns, name)
		cppname = None if name in nocpp else "%s.%s.cpp" % (ns, name)


		#---------------------------------------------
		makeheader(headername, """\
#include "{ns}.h"
namespace {ns} {{;;
//====================================================
//= struct {ns}::{name}
//====================================================
struct {name} {base}
{{
}};
}}//namespace {ns}\
""".format(ns = ns, name = name
			, base = "" if name == "object" else ": public object"
		))
		#---------------------------------------------
		
		if cppname == None:
			continue
		
		#---------------------------------------------
		makecpp(cppname, """\
#include "{ns}.h"
using namespace {ns};
//====================================================
//= struct {ns}::{name}
//====================================================
""".format(ns = ns, name = name
		))
		#---------------------------------------------

#=====================================================
#= entry point
#=====================================================
if __name__ == "__main__":
	run()	
