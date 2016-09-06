# -*- mode: makefile-gmake -*-

all:
.PHONY: all

CXX := cxx
CXXFLAGS := -O2 -Wall

LDFLAGS:=
LIBS:=

all: diff
diff: diff.o
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)
diff.o: diff.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<
