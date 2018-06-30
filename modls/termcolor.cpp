// -*- coding: utf-8 -*-
#include <cstdio>
#ifdef USE_TERMINFO
# include <ncurses/term.h>
#endif

#include "colored_string.h"

#ifdef USE_TERMINFO
static void tputs0(const char* s) {
  if (!s) return;
  while (*s) std::putchar(*s++);
  // tputs(s,1,putchar);
}

void init_termcolor() {
  setupterm(0, 1, 0);
}
#endif

void cc::clear() {
#ifdef USE_TERMINFO
  if (exit_attribute_mode)
    tputs0(exit_attribute_mode);
  else {
#endif
    std::putchar('\33');
    std::putchar('[');
    std::putchar('m');
#ifdef USE_TERMINFO
  }
#endif
}


static void put_forecolor(byte c) {
  if (c & 010) {
    std::putchar('1');
    std::putchar(';');
  }
  std::putchar('3');
  std::putchar('0'+(c&07));
}
static void put_backcolor(byte c) {
  if (c & 010) {
    std::putchar('5');
    std::putchar(';');
  }
  std::putchar('4');
  std::putchar('0'+(c&07));
}

void cc::set_color(word c) {
  cc::clear();
  if (c == cc::cc_default) return;
  byte fc = c & 0xff;
  byte bc = c >> 8 & 0xff;

  bool fF = fc != cc::def;
  bool fB = bc != cc::def;
#ifdef USE_TERMINFO
  if (fF && fc < max_colors && set_a_foreground) {
    tputs0(tparm(set_a_foreground, fc));
    fF = false;
  }
  if (fB && bc < max_colors && set_a_background) {
    tputs0(tparm(set_a_background, bc));
    fB = false;
  }
  if (fF || fB) {
#endif
    std::putchar('\33');
    std::putchar('[');
    if (fF) put_forecolor(fc);
    if (fF && fB) std::putchar(';');
    if (fB) put_backcolor(bc);
    std::putchar('m');
#ifdef USE_TERMINFO
  }
#endif
}