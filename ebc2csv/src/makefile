NAME = ebc2csv
BINDIR = ../bin
OBJDIR = ../obj
TARGET = $(BINDIR)/$(NAME).so
EXEOUT = $(BINDIR)/$(NAME)

SOURCES = $(wildcard *.cpp)
OBJECTS = $(addprefix $(OBJDIR)/,$(SOURCES:.cpp=.o))
DEPENDS = $(OBJECTS:.o=.d)

CXXFLAGS = -Wall -O3 -MMD -MP -fPIC

DLLFLAGS = -static -shared
EXEFLAGS = -static 
LIBS = -lpthread -lebc2json

CPPROOT = ../..
ATDD = $(CPPROOT)/atd.d

INCDIRS = \
	-I $(ATDD)/src
LIBDIRS = \
	-L /usr/lib64 \
	-L $(ATDD)/bin
IMPLIBS = \
	$(ATDD)/bin/libebc2json.a

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