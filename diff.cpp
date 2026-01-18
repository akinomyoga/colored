#include <clocale>
#include <cwchar>
#include <cwctype>
#include <iostream>
#include <string>
#include <locale>
#include <vector>
#include <utility>
#include <algorithm>

static const std::size_t DETAILED_DIFF_LIMIT = 10 * 1024 * 1024;

// from lwiki/lib/lib.ldiff.php
template<typename F, bool swapped = false>
static void ldiff_getpath_wu(
  std::vector<std::pair<int, int> >& _path,
  F const& equals, int M, int N
){
  if (!swapped && M < N) {
    ldiff_getpath_wu<F, true>(_path, equals, N, M);
    return;
  }

  /* Wu のアルゴリズム
   *
   * p = yskip の回数。
   *   できるだけ p の小さな経路を探せば良い。
   *   p についてループして初めに到達した経路を解とすれば良い。
   *
   * k = 対角線番号 (0 <= k <= M + N)
   *
   *     +-- M ---------------+
   *   + +----------------+---+
   *   | |\               :\  |\
   *   N | \              : \ | \
   *   | |  \             :  \|  \
   *   + +---+----------------+   +
   *     :   :            :   :   :
   *    k=0 k=N        k=M-N k=M k=M+N
   *
   *   或る p の時に各対角線上で最も進める位置(x, y)を考える。
   *
   * 各対角線上で
   *   q = k - N + p = xskipの回数,
   *   s[q].path = 一致開始点の集合,
   *   s[q].m = 合計一致長,
   *   x = s[q].m + q,
   *   y = s[q].m + p.
   *
   */

  struct diagonal {
    int m {0};
    std::vector<std::pair<int, int> > path;
  };
  std::vector<diagonal> s;
  s.reserve(M + 1);

  for (int p = 0; p <= N; p++) {
    int const qN = std::min(M, M - N + 2 * p + 1);
    s.resize(qN);
    for (int q = 0; q < qN; q++) {
      // 左から来た方が速い場合
      if (q > 0 && s[q].m < s[q-1].m) s[q] = s[q-1];

      // 共通部分の読み取り
      int const x0 = s[q].m + q;
      int const y0 = s[q].m + p;
      int x = x0, y = y0;
      for (; x < M && y < N && (swapped? equals(y, x): equals(x, y)); x++, y++);

      std::vector<std::pair<int, int> >& path = s[q].path;
      if (y != y0)
        path.push_back(swapped? std::make_pair(y0, x0): std::make_pair(x0, y0));

      // 到達確認
      if (y == N) {
        _path = std::move(path);
        return;
      }

      s[q].m = y - p;
    }
  }
}

const wchar_t* termcap_sgr0 = L"\x1b[m";
const wchar_t* termcap_other = L"\x1b[;1m";
const wchar_t* termcap_header = L"\x1b[;38;5;21;48;5;189m";
const wchar_t* termcap_hank_comment = L"\x1b[;1;30;48;5;189m";

const wchar_t* termcap_begin_cntrl = L"\x1b[7m";
const wchar_t* termcap_end_cntrl = L"\x1b[27m";

const wchar_t* termcap_head = L"\x1b[;38;5;25;48;5;189m";

const wchar_t* termcap_begin_rline = L"\x1b[;30;48;5;218m";
const wchar_t* termcap_begin_aline = L"\x1b[;30;48;5;154m";
const wchar_t* termcap_begin_rback = L"\x1b[;30;48;5;225m";
const wchar_t* termcap_begin_aback = L"\x1b[;30;48;5;193m";
const wchar_t* termcap_begin_rword = L"\x1b[;30;48;5;218m";//217,225
const wchar_t* termcap_begin_aword = L"\x1b[;30;48;5;154m";//112,193

struct diff_processor {

  void putchar_escaped(wchar_t ch) {
    if (std::iswcntrl(ch)) {
      std::wcout << termcap_begin_cntrl;
      if (0 <= ch && ch < 32) {
        std::wcout.put(L'^');
        std::wcout.put(wchar_t(L'A' - 1 + ch));
      } else if (128 <= ch && ch < 160) {
        std::wcout.put(L'_');
        std::wcout.put(wchar_t(L'A' - 1 + ch));
      } else {
        std::wcout.put(ch);
      }
      std::wcout << (section_prefix? section_prefix: termcap_sgr0);
    } else {
      std::wcout.put(ch);
    }
  }

  bool isBeginningOfLine {true};
  wchar_t section_chead {' '};
  const wchar_t* section_prefix {nullptr};

  void section_setup(wchar_t chead, const wchar_t* prefix) {
    isBeginningOfLine = true;
    section_chead = chead;
    section_prefix = prefix;
  }
  void section_putchar(wchar_t ch) {
    if (ch == L'\n') {
      if (isBeginningOfLine) {
        std::wcout << termcap_head;
        std::wcout.put(section_chead);
      }

      std::wcout << termcap_sgr0;
      std::wcout.put(L'\n');
      isBeginningOfLine = true;
    } else {
      if (isBeginningOfLine) {
        isBeginningOfLine = false;
        std::wcout << termcap_head;
        std::wcout.put(section_chead);
        if (section_prefix) std::wcout << section_prefix;
      }
      putchar_escaped(ch);
    }
  }

  void section_reset(){
    if (!isBeginningOfLine) {
      std::wcout << termcap_sgr0;
      std::wcout.put(L'\n');
    }

    isBeginningOfLine = true;
    section_chead = ' ';
    section_prefix = nullptr;
  }

  void putword(std::wstring const& text, std::size_t& index, std::size_t until, const wchar_t* prefix) {
    if (index < until) {
      using std::swap;
      swap(section_prefix, prefix);
      std::wcout << section_prefix;

      while (index < until)
        section_putchar(text[index++]);

      swap(section_prefix, prefix);
      std::wcout << section_prefix;
    }
  }

  void output_detailed_diff(std::wstring const& removed, std::wstring const& added) {
    // 巨大な時は処理を省略
    if (removed.size() * added.size() > DETAILED_DIFF_LIMIT) {
      section_setup(L'-', termcap_begin_rback);
      for (wchar_t c: removed) section_putchar(c);
      section_reset();
      section_setup(L'+', termcap_begin_aback);
      for (wchar_t c: added) section_putchar(c);
      section_reset();
      return;
    }

    std::vector<std::pair<int,int> > path;
    ldiff_getpath_wu(path, [&removed, &added] (int i, int j) { return removed[i] ==  added[j]; }, removed.size(), added.size());
    std::size_t index;

    // removed
    index = 0;
    section_setup(L'-', termcap_begin_rback);
    for (auto const& spec: path) {
      putword(removed, index, spec.first, termcap_begin_rword);

      while (index < removed.size() && removed[index] == added[spec.second - spec.first + index]) {
        section_putchar(removed[index++]);
      }
    }
    putword(removed, index, removed.size(), termcap_begin_rword);
    section_reset();

    // add
    section_setup(L'+', termcap_begin_aback);
    index = 0;
    for (auto const& spec: path) {
      putword(added, index, spec.second, termcap_begin_aword);
      while (index < added.size() && added[index] == removed[spec.first - spec.second + index]) {
        section_putchar(added[index++]);
      }
    }
    putword(added, index, added.size(), termcap_begin_aword);
    section_reset();
  }

  std::wstring removed;
  std::wstring added;

  void flush_buffered_lines() {
    if (!removed.empty()) {
      if (!added.empty()) {
        output_detailed_diff(removed, added);
        added.clear();
      } else {
        section_setup(L'-', termcap_begin_rline);
        for (wchar_t ch: removed) section_putchar(ch);
        section_reset();
      }

      removed.clear();
    }
  }

  void output_content_line(std::wstring const& line, const wchar_t* sgr = nullptr) {
    bool ishead = true;
    if (sgr) section_prefix = sgr;
    for (wchar_t ch: line) {
      if (ishead) {
        std::wcout << termcap_head;
        putchar_escaped(ch);
        std::wcout << (sgr? sgr: termcap_sgr0);
        ishead = false;
      } else
        putchar_escaped(ch);
    }

    if (sgr) {
      section_prefix = termcap_sgr0;
      std::wcout << termcap_sgr0;
    }
    std::wcout.put(L'\n');
  }

  void output_hank_header(std::wstring const& line) {
    std::size_t const needle = line.find(L" @@", 3);
    std::size_t const end = needle == line.npos ? line.size() : needle + 3;

    std::size_t index = 0;
    std::wcout << termcap_header;
    for (; index < end; index++) putchar_escaped(line[index]);

    if (index < line.size()) {
      std::wcout << termcap_hank_comment;
      for (; index < line.size(); index++) putchar_escaped(line[index]);
    }

    std::wcout << termcap_sgr0;
    std::wcout.put(L'\n');
  }

  enum line_type {
    other_line,
    added_line,
    removed_line,
    unchanged_line,
    hank_header,
  };

  void update_line_type(line_type& type, std::wstring const& line) {
    if (type == other_line)
      if (line.compare(0, 4, L"+++ ") == 0 || line.compare(0, 4, L"--- ") == 0)
        return;

    if (line.compare(0, 3, L"@@ ") == 0)
      type = hank_header;
    else if (line[0] == L'-')
      type = removed_line;
    else if (line[0] == L'+')
      type = added_line;
    else if (line[0] == L' ')
      type = unchanged_line;
    else
      type = other_line;
  }

  void process() {
    line_type type = other_line;
    std::wstring line;
    while (std::getline(std::wcin, line)) {
      update_line_type(type, line);

      switch (type) {
      case added_line:
        if (!removed.empty()) {
          added.append(line, 1, line.size() - 1);
          added.push_back(L'\n');
        } else
          output_content_line(line, termcap_begin_aline);
        break;

      case removed_line:
        if (!added.empty()) flush_buffered_lines();
        removed.append(line, 1, line.size() - 1);
        removed.push_back(L'\n');
        break;

      case unchanged_line:
        if (!removed.empty()) flush_buffered_lines();
        output_content_line(line);
        break;

      case hank_header:
        if (!removed.empty()) flush_buffered_lines();
        output_hank_header(line);
        break;

      case other_line:
        if (!removed.empty()) flush_buffered_lines();
        std::wcout << termcap_other;
        for (wchar_t ch: line) putchar_escaped(ch);
        std::wcout << termcap_sgr0;
        std::wcout.put(L'\n');
        break;
      }
    }

    if (!removed.empty()) flush_buffered_lines();
  }

};

int main() {
  std::ios_base::sync_with_stdio(false);
#if defined(__MINGW32__)
  // * 2026-01-19 We have been using this branch also for defined(__CYGWIN__).
  //   However, a recent version of Cygwin seems to have started to offer the
  //   proper C++ interfaces, and "std::setlocale(LC_CTYPE, "")" stopped
  //   working.
  //
  //   However, There does not seem to be any pre-defined macros that can be
  //   used to switch the code depending on the Cygwin version, which is
  //   strange.  Because Cygwin can be updated regardless of the Windows
  //   system, we assume that no user uses an old version of Cygwin, and
  //   entirely switch to the correct version.
  std::setlocale(LC_CTYPE, "");
#else
  std::locale::global(std::locale(std::locale::classic(), std::locale(""), std::locale::ctype));
  std::wcout.imbue(std::locale());
  std::wcin.imbue(std::locale());
#endif

  diff_processor proc;
  proc.process();

  std::wcout << std::flush;
  return 0;
}
