# -*- mode: makefile-gmake -*-

all:
.PHONY: all

# compile

CXX := g++
CXXFLAGS := -O2 -Wall -std=c++11

LDFLAGS:=
LIBS:=

all: diff.exe
diff.exe: diff.o
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)
diff.o: diff.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# install

.PHONY: install

ifneq ($(MWGDIR),)
  PREFIX := $(MWGDIR)
else
  PREFIX := $(HOME)/.mwg
endif

share_directory := $(PREFIX)/share/colored
directories += $(share_directory)

bin_directory := $(PREFIX)/bin
directories += $(bin_directory)

install: $(share_directory)/diff.exe
$(share_directory)/diff.exe: diff.exe | $(share_directory)
	cp $< $@

install: $(bin_directory)/colored
$(bin_directory)/colored: colored | $(bin_directory)
	sed "/^colored_share_directory=/c colored_share_directory='$(share_directory)'" $< > $@ && chmod +x $@

# misc

$(directories):
	mkdir -p $@
