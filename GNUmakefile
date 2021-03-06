# -*- mode: makefile-gmake -*-

all:
.PHONY: all

#------------------------------------------------------------------------------
#
# config
#

make_config.mk: ./configure
	./configure
include make_config.mk

#------------------------------------------------------------------------------
#
# compile
#

build_directory = out
directories += $(build_directory)

-include $(wildcard $(build_directory)/*.dep)
CPPFLAGS = -MD -MP -MF $(@:.o=.dep) -MP

# diff

$(build_directory)/diff.o: diff.cpp | $(build_directory)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<
$(build_directory)/diff.exe: $(build_directory)/diff.o
	$(CXX) $(CPPFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)
all: $(build_directory)/diff.exe


# modls

modls_CPPFLAGS = $(CPPFLAGS) $(config_CPPFLAGS)
modls_CXXFLAGS := $(CXXFLAGS) $(config_CXXFLAGS)
modls_LIBS := $(config_LIBS) $(LIBS)

$(build_directory)/modls.o: modls/modls.cpp | $(build_directory)
	$(CXX) $(modls_CPPFLAGS) $(modls_CXXFLAGS) -c -o $@ $<
$(build_directory)/termcolor.o: modls/termcolor.cpp | $(build_directory)
	$(CXX) $(modls_CPPFLAGS) $(modls_CXXFLAGS) -c -o $@ $<
$(build_directory)/modls.exe: $(build_directory)/modls.o $(build_directory)/termcolor.o
	$(CXX) $(modls_CXXFLAGS) $(LDFLAGS) -o $@ $^ $(modls_LIBS)
all: $(build_directory)/modls.exe


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
$(share_directory)/diff.exe: $(build_directory)/diff.exe | $(share_directory)
	cp $< $@

install: $(share_directory)/modls.exe
$(share_directory)/modls.exe: $(build_directory)/modls.exe | $(share_directory)
	cp $< $@

install: $(bin_directory)/colored
$(bin_directory)/colored: colored | $(bin_directory)
	share_directory='$(share_directory)' bash make.sh install-colored $< $@

install:
	bash make.sh update

#------------------------------------------------------------------------------
#
# misc
#

$(directories):
	mkdir -p $@

clean:
	-rm -rf out/*

# PWDNAME:=$(subst $(dir $(PWD)),,$(PWD))
# dist: dist_xz
# #dist: dist_xz dist_gz
# .PHONY: dist_xz dist_gz
# dist_xz:
# 	DATE=`date +%Y%m%d` && cd .. && tar cavf modls.$$DATE.tar.xz --exclude='*~' --exclude='*.o' --exclude='modls$(EXEEXT)' --exclude=*/backup "$(PWDNAME)"
# dist_gz:
# 	DATE=`date +%Y%m%d` && cd .. && tar cavf modls.$$DATE.tar.gz --exclude='*~' --exclude='*.o' --exclude='modls$(EXEEXT)' --exclude=*/backup "$(PWDNAME)"
