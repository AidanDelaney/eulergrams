# ship these headers in the "make dist" target
#EXTRA_DIST = JSONAbstractDescription.h

bin_PROGRAMS = eulergrams
eulergrams_SOURCES = JSONAbstractDescription.cpp Main.cpp
eulergrams_CXXFLAGS=-std=c++11

# uncool
eulergrams_LDADD=-L/usr/local/lib -lcola -lvpsc -lboost_program_options

# Rules for the test code (use `make check` to execute)
TESTS = tests
check_PROGRAMS = $(TESTS)
# FIXME: make libeulergrams so we don't have to list the source here.
tests_SOURCES  = Tests.cpp TestJSONImport.cpp JSONAbstractDescription.cpp TestContainmentHierarchy.cpp
tests_CXXFLAGS = $(CPPUNIT_CFLAGS) -std=c++11
tests_LDFLAGS  = $(CPPUNIT_LIBS) -L/usr/local/lib -lcola -lvpsc
