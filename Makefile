# Makefile

BINDIR=~/.mwg
PWDNAME:=$(subst $(dir $(PWD)),,$(PWD))

.PHONY: all dist install
all: $(BINDIR) $(BINDIR)/modls

$(BINDIR):
	test -d $(BINDIR) || mkdir $(BINDIR)

$(BINDIR)/modls: modls.cpp scanner.h colored_string.h
	g++ -o$@ $<

install: $(BINDIR)/modls
	./install.sh

dist:
	cd .. && tar cavf mwg.modls.tar.lzma --exclude='*~' "$(PWDNAME)"
	cd .. && tar cavf mwg.modls.tar.bz2 --exclude='*~' "$(PWDNAME)"
