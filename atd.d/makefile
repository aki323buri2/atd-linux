NAME = test
BINDIR = ./bin
OBJDIR = ./obj
TARGET = $(BINDIR)/$(NAME).so
EXEOUT = $(BINDIR)/$(NAME).exe

SOURCES = $(wildcard *.cpp)
OBJECTS = $(addprefix $(OBJDIR)/,$(SOURCES:.cpp=.o))
DEPENDS = $(OBJECTS:.o=.d)

CXXFLAGS = -Wall -O3 -MMD -MP -fPIC

DLLLDFLAGS = -static -shared
EXELDFLAGS = -static
LIBS = -lpthread

CPPROOT = ..
ATDD = $(CPPROOT)/atd.d
AUID = $(CPPROOT)/aui.d
EBCD = $(CPPROOT)/ebc2json
REGEXD = $(CPPROOT)/regex-2.7-bin
INCDIRS = \
	-I $(ATDD) \
	-I $(AUID) \
	-I $(EBCD) \
	-I $(REGEXD)/include
LIBDIRS = \
	-L /usr/lib64 \
	-L $(ATDD)/bin \
	-L $(AUID)/bin \
	-L $(EBCD)/bin \
	-L $(REGEXD)/lib
# IMPLIBS = \
# 	$(ATDD)/bin/libatd.a \
# 	$(AUID)/bin/libaui.a \
# 	$(EBCD)/bin/libebc2json.a 

$(TARGET): $(OBJECTS) $(IMPLIBS) $(EXEOUT)
	@ [ -d $(BINDIR) ] | mkdir -p $(BINDIR)
	$(CXX) $(DLLLDFLAGS) $(LIBDIRS) -o $@ $(OBJECTS) $(LIBS)
	strip $@ --strip-all 
	#静的ライブラリ
	ar rsv $(BINDIR)/lib$(NAME).a $^

$(EXEOUT): $(OBJECTS) $(IMPLIBS)
	@ [ -d $(BINDIR) ] | mkdir -p $(BINDIR)
	$(CXX) $(EXELDFLAGS) $(LIBDIRS) -o $@ $(OBJECTS) $(LIBS)
	strip $@ --strip-all 


$(OBJDIR)/%.o: %.cpp
	@ [ -d $(OBJDIR) ] | mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(INCDIRS) -o $@ -c $<

clean:
	$(RM) $(TARGET) $(OBJECTS) $(DEPENDS)

-include $(DEPENDS)