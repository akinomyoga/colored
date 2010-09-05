# Makefile

BINDIR=~/.mwg

.PHONY: all
all: $(BINDIR)/modls

$(BINDIR):
	test -d $(BINDIR) || mkdir $(BINDIR)

$(BINDIR)/modls: modls.cpp scanner.h colored_string.h $(BINDIR)
	g++ modls.cpp -o $@
