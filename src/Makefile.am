# ship these headers in the "make dist" target
#EXTRA_DIST = JSONAbstractDescription.h

bin_PROGRAMS = eulergrams
eulergrams_SOURCES = JSONAbstractDescription.cpp Main.cpp
eulergrams_CXXFLAGS=-std=c++11

# uncool
eulergrams_LDADD=-L/usr/local/lib -lcola -lvpsc -lboost_program_options
