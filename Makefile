# Makefile

BINDIR=$(HOME)/.mwg
PWDNAME:=$(subst $(dir $(PWD)),,$(PWD))

CXXFLAGS:=-DUSE_TERMINFO

all: $(BINDIR) $(BINDIR)/modls$(EXEEXT)
.PHONY: all dist install clear

clear:
	-rm -f *.o

$(BINDIR):
	test -d $(BINDIR) || mkdir $(BINDIR)

modls.o: modls.cpp scanner.h colored_string.h
	g++ $(CXXFLAGS) -c -o$@ $<
termcolor.o: termcolor.cpp colored_string.h
	g++ $(CXXFLAGS) -c -o$@ $<
$(BINDIR)/modls$(EXEEXT): modls.o termcolor.o
	g++ -o$@ $^ -lncurses

install: $(BINDIR)/modls$(EXEEXT)
	./install.sh

dist:
	DATE=`date +%Y%m%d` && cd .. && { \
  tar cavf modls.$$DATE.tar.xz --exclude='*~' --exclude='*.o' "$(PWDNAME)"; \
  tar cavf modls.$$DATE.tar.gz --exclude='*~' --exclude='*.o' "$(PWDNAME)"; \
}
