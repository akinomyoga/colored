# -*- mode: makefile-gmake -*-

all:
.PHONY: all

#------------------------------------------------------------------------------
#
# config
#

use_ncurses := yes

CXXFLAGS := -O3 -Wall -Wno-parentheses -std=gnu++11
# CXX      := g++
# LDFLAGS  :=
# LIBS     :=

#------------------------------------------------------------------------------
#
# compile
#

build_directory = out
directories += $(build_directory)

-include $(wildcard $(build_directory)/*.dep)
CPPFLAGS = -MD -MF $(@:.o=.dep) -MP


# diff

$(build_directory)/diff.o: diff.cpp | $(build_directory)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<
diff.exe: $(build_directory)/diff.o
	$(CXX) $(CPPFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)
all: diff.exe


# modls

ifneq ($(use_ncurses),no)
  modls_CXXFLAGS := $(CXXFLAGS) -DUSE_TERMINFO
  modls_LIBS     := -lncurses $(LIBS)
endif

$(build_directory)/modls.o: modls/modls.cpp | $(build_directory)
	$(CXX) $(CPPFLAGS) $(modls_CXXFLAGS) -c -o $@ $<
$(build_directory)/termcolor.o: modls/termcolor.cpp | $(build_directory)
	$(CXX) $(CPPFLAGS) $(modls_CXXFLAGS) -c -o $@ $<
modls.exe: $(build_directory)/modls.o $(build_directory)/termcolor.o
	$(CXX) $(modls_CXXFLAGS) $(LDFLAGS) -o $@ $^ $(modls_LIBS)
all: modls.exe


#------------------------------------------------------------------------------
#
# install
#

install:
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

install: $(share_directory)/modls.exe
$(share_directory)/modls.exe: modls.exe | $(share_directory)
	cp $< $@

install: $(bin_directory)/colored
$(bin_directory)/colored: colored | $(bin_directory)
	ls_supports_time_style=$$(ls --help | grep -qe '--time-style=' && echo 1); \
	sed -e "/^colored_share_directory=/c colored_share_directory='$(share_directory)'" \
	    -e "s/^\(colored_ls_supports_time_style=\).*/\1$$ls_supports_time_style/" $< > $@ && chmod +x $@

#------------------------------------------------------------------------------
#
# misc
#

$(directories):
	mkdir -p $@

clean:
	-rm -rf out/*
