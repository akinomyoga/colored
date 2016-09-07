# -*- mode: makefile-gmake -*-

all:
.PHONY: all

# compile

CXX := cxx
CXXFLAGS := -O2 -Wall

LDFLAGS:=
LIBS:=

all: diff
diff: diff.o
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)
diff.o: diff.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# install

.PHONY: install
PREFIX := $(HOME)/.mwg

share_directory := $(PREFIX)/share/colored
directories += $(share_directory)

bin_directory := $(PREFIX)/bin
directories += $(bin_directory)

install: $(share_directory)/diff
$(share_directory)/diff: diff | $(share_directory)
	cp $< $@

install: $(bin_directory)/colored
$(bin_directory)/colored: colored | $(bin_directory)
	sed "/^colored_share_directory=/c colored_share_directory=$(share_directory)" $< > $@ && chmod +x $@

# misc

$(directories):
	mkdir -p $@
