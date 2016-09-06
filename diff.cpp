#include <iostream>
#include <string>
#include <locale>
#include <vector>
#include <utility>
#include <cwchar>

// from lwiki/lib/lib.ldiff.php
template<typename T>
static void _ldiff_getpath_wu(
  std::vector<std::pair<int, int> >& _path,
  T const& _arr1,
  T const& _arr2
){
  T const* parr1 = &_arr1;
  T const* parr2 = &_arr2;
  int M = parr1->size();
  int N = parr2->size();

  bool const swapped = M < N;
  if(swapped){
    using namespace std;
    swap(parr1, parr2);
    swap(M, N);
  }

  T const& arr1 = *parr1;
  T const& arr2 = *parr2;

  // Wu のアルゴリズム

  // $p: $arr2 のskip回数 (これの数でループを回す)
  // $k: 対角線番号
  // $x = $y+$k-N        ($arr1 内の index, 0<=$x<M)
  // $y = $y              ($arr2 内の index, 0<=$y<N)
  //
  // assert(0<=$k&&$k<=N+M);
  // assert($y>=$p);

  struct hoge {
    int y;
    std::vector<std::pair<int, int> > path;
  };
  std::vector<hoge> s(M + N + 1); // ToDo check preferred size

  // s[k].y
  //   k ... 対角線番号 0 <= k < M+N
  //   各対角線について skip 回数 $p 回で何処の $y まで到達できるか?

  for (int p = 0; p <= N; p++) {
    for (int k = N - p; k <= M + p + 1; k++) {
      int y = 0;
      std::vector<std::pair<int, int> > path;

      if (p > 0 && k < M + p) {
        // 上からの寄与
        y = s[k+1].y + 1;
        path = std::move(s[k+1].path); // 後で上書きされるので流用可
      }

      if (k > N - p && y < s[k-1].y) {
        // 左からの寄与
        y = s[k-1].y;
        path = s[k-1].path; // 後で使われるのでクローン
      }

      // ※上・左どちらの寄与もないのは原点 (x,y)=(0,0) にいる時のみ。
      //   この時は y == 0 && path.empty() で良い。

      // 進めるだけ進む (進めたら開始点を記録)
      {
        int isfirst = true;
        int x = y + k - N;
        while (y < N && x < M && arr1[x]==arr2[y]) {
          if (isfirst) {
            isfirst = false;
            path.push_back(swapped? std::make_pair(y, x): std::make_pair(x, y));
          }
          y++;
          x++;
        }
      }

      if (y == N) {
        _path = std::move(path);
        return;
      }

      s[k].y = y;
      s[k].path = std::move(path);
    }
  }
}

const wchar_t* termcap_sgr0 = L"\x1b[m";
const wchar_t* termcap_other = L"\x1b[;1m";
const wchar_t* termcap_header = L"\x1b[;38;5;21;48;5;189m";
const wchar_t* termcap_hank_comment = L"\x1b[;1;48;5;189m";

const wchar_t* termcap_begin_cntrl = L"\x1b[7m";
const wchar_t* termcap_end_cntrl = L"\x1b[27m";

const wchar_t* termcap_head = L"\x1b[;38;5;25;48;5;254m";

const wchar_t* termcap_begin_rline = L"\x1b[;48;5;225m";
const wchar_t* termcap_begin_aline = L"\x1b[;48;5;193m";

const wchar_t* termcap_begin_rword = L"\x1b[;1;48;5;217m";
const wchar_t* termcap_begin_aword = L"\x1b[;1;48;5;118m";

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

  bool isBeginningOfLine;
  wchar_t section_chead;
  const wchar_t* section_prefix;

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
    std::vector<std::pair<int,int> > path;
    _ldiff_getpath_wu(path, removed, added);
    std::size_t index;

    // removed
    index = 0;
    section_setup(L'-', termcap_begin_rline);
    for (auto const& spec: path) {
      putword(removed, index, spec.first, termcap_begin_rword);

      while (index < removed.size() && removed[index] == added[spec.second - spec.first + index]) {
        section_putchar(removed[index++]);
      }
    }
    putword(removed, index, removed.size(), termcap_begin_rword);
    section_reset();

    // add
    section_setup(L'+', termcap_begin_aline);
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
    for (wchar_t ch: line) {
      if (ishead) {
        std::wcout << termcap_head;
        putchar_escaped(ch);
        std::wcout << (sgr? sgr: termcap_sgr0);
        ishead = false;
      } else
        putchar_escaped(ch);
    }

    if (sgr) std::wcout << termcap_sgr0;
    std::wcout.put(L'\n');
  }

  void output_hank_header(std::wstring const& line) {
    std::size_t const needle = line.find(L" @@", 3);
    std::size_t const end = needle == line.npos? line.size(): needle + 3;

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
  }

};

int main() {
  std::setlocale(LC_CTYPE, "");
  std::ios_base::sync_with_stdio(false);
  std::locale::global(std::locale(std::locale::classic(), std::locale(""), std::locale::ctype));
  std::wcout.imbue(std::locale());
  std::wcin.imbue(std::locale());

  diff_processor proc;
  proc.process();

  std::wcout << std::flush;
}
