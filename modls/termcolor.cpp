// -*- coding: utf-8 -*-
#include <cstdio>
#ifdef USE_TERMINFO
# include USE_TERMINFO
#endif

#include "colored_string.h"

static const bool aix_colors = true;
static const bool sgr_bright = false;

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

static byte reduce_color(byte c) {
  if (c < 8)
    return c;
  else if (c < 16) {
    if (aix_colors || sgr_bright) return c;
    return c & 7;
  } else if (c < 232) {
    c -= 16;
    int r = c / 36;
    int g = c / 6 % 6;
    int b = c % 6;
    if (r == g && g == b) {
      // 0 1 2 3 4 5
      // B g g s s W
      if (aix_colors || sgr_bright)
        return r == 0 ? 0 : r == 5 ? 15 : r >= 3 ? 7 : 8;
      else
        return r >= 3 ? 7 : 0;
    } else {
      int const mx = std::max(r, std::max(g, b));
      int const mn = std::min(r, std::min(g, b));
      int const md = (mx + mn) / 2;
      int const c8 = (r > md) | (g > md) << 1 | (b > md) << 2;
      if (mx >= 3 && (aix_colors || sgr_bright)) return 8 + c8;
      return c8;
    }
  } else {
    c -= 232; // c = 0..23
    if (aix_colors || sgr_bright)
      return c < 4 ? 0 : 20 <= c ? 15 : c >= 12 ? 7 : 8;
    else
      return c >= 12 ? 7 : 0;
  }
}

// キャンセルする為のフラグ
// 0: キャンセルの必要はない
// -1: キャンセルできない
// -2: 予約 (意味は未定)
static int sgr_canceller[] = {
  -1, 22, 22, 23, 24, 25, 25, 27, 28, 29,
  0 , 10, 10, 10, 10, 10, 10, 10, 10, 10,
  23, 24, 0 , 0 , 0 , 0 , 50, 0 , 0 , 0 ,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  0 , 54, 54, 55, 0 , 0 , -2, -2, -2, -2,
  65, 65, 65, 65, 65, 0 , 65, 65, 65, 65,
};

struct sgr_writer {
  int count;

public:
  sgr_writer(): count(0) {}
  ~sgr_writer() {
    if (count) {
      std::putchar('m');
      count = 0;
    }
  }

public:
  void put_number(unsigned number) {
    if (!number) return;
    put_number(number / 10);
    std::putchar('0' + number % 10);
  }
  void put_param(unsigned param) {
    if (count++ == 0) {
      std::putchar('\x1b');
      std::putchar('[');
    } else {
      std::putchar(';');
    }
    put_number(param);
  }

private:
  void put_color(byte c, bool isbg) {
    if (c < 8) {
      put_param((isbg ? 40 : 30) + c);
      return;
    }

    if (c < 16 && aix_colors) {
      put_param((isbg ? 100 : 90) + (c & 7));
      return;
    }

    if (c < 16 && sgr_bright) {
      put_param(isbg ? 5 : 1);
      put_param((isbg ? 40 : 30) + (c & 7));
      return;
    }

    // workaround: 256色対応のない環境
    put_color(reduce_color(c), isbg);

    put_param(isbg ? 48 : 38);
    put_param(5);
    put_param(c);

    // workaround: 256色対応のない環境
    put_param(25);
    if (sgr_canceller[c] > 0)
      put_param(sgr_canceller[c]);
  }

public:
  void put_forecolor(byte c) {
    put_color(c, false);
  }
  void put_backcolor(byte c) {
    put_color(c, true);
  }
};

void cc::set_color(word c) {
  if (c == cc::cc_default) {
    cc::clear();
    return;
  }

  byte fc = c & 0xff;
  byte bc = c >> 8 & 0xff;

  bool fF = fc != cc::def;
  bool fB = bc != cc::def;
#ifdef USE_TERMINFO
  if (
    (!fF || (fc < max_colors && set_a_foreground)) &&
    (!fB || (bc < max_colors && set_a_background))
  ) {
    cc::clear();
    if (fF && fc < max_colors && set_a_foreground) {
      tputs0(tparm(set_a_foreground, fc));
      fF = false;
    }
    if (fB && bc < max_colors && set_a_background) {
      tputs0(tparm(set_a_background, bc));
      fB = false;
    }
    return;
  }
#endif

  sgr_writer writer;
  writer.put_param(0);
  if (fF) writer.put_forecolor(fc);
  if (fB) writer.put_backcolor(bc);
}
