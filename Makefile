# Makefile

SHARE=$(HOME)/.mwg/share/modls

CXXFLAGS:=-DUSE_TERMINFO

all: modls$(EXEEXT)
.PHONY: all dist install clear

clear:
	-rm -f *.o

modls.o: modls.cpp scanner.h colored_string.h
	g++ $(CXXFLAGS) -c -o$@ $<
termcolor.o: termcolor.cpp colored_string.h
	g++ $(CXXFLAGS) -c -o$@ $<
modls$(EXEEXT): modls.o termcolor.o
	g++ -o$@ $^ -lncurses

$(SHARE):
	test -d $(SHARE) || mkdir -p $(SHARE)
$(SHARE)/modls$(EXEEXT): modls$(EXEEXT)
	cp -p $< $@
install: $(SHARE) $(SHARE)/modls$(EXEEXT)
	./install.sh

PWDNAME:=$(subst $(dir $(PWD)),,$(PWD))
dist:
	DATE=`date +%Y%m%d` && cd .. && { \
  tar cavf modls.$$DATE.tar.xz --exclude='*~' --exclude='*.o' --exclude='modls$(EXEEXT)' --exclude=*/backup "$(PWDNAME)"; \
  tar cavf modls.$$DATE.tar.gz --exclude='*~' --exclude='*.o' --exclude='modls$(EXEEXT)' --exclude=*/backup "$(PWDNAME)"; \
}
