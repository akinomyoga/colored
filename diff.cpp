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

const wchar_t* termcap_begin_cntrl = L"\x1b[1m";
const wchar_t* termcap_end_cntrl = L"\x1b[22m";

const wchar_t* termcap_begin_rline = L"\x1b[48;5;225m";
const wchar_t* termcap_end_rline = L"\x1b[49m";
const wchar_t* termcap_begin_aline = L"\x1b[48;5;193m";
const wchar_t* termcap_end_aline = L"\x1b[49m";

const wchar_t* termcap_begin_rword = L"\x1b[1;48;5;217m";
const wchar_t* termcap_end_rword = L"\x1b[22;48;5;225m";
const wchar_t* termcap_begin_aword = L"\x1b[1;48;5;118m";
const wchar_t* termcap_end_aword = L"\x1b[22;48;5;193m";

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
      std::wcout << termcap_end_cntrl;
    } else {
      std::wcout.put(ch);
    }
  }

  bool isBeginningOfLine;
  wchar_t section_chead;
  const wchar_t* section_prefix;
  const wchar_t* section_suffix;

  void section_setup(wchar_t chead, const wchar_t* prefix, const wchar_t* suffix) {
    isBeginningOfLine = true;
    section_chead = chead;
    section_prefix = prefix;
    section_suffix = suffix;
  }
  void section_putchar(wchar_t ch) {
    if (ch == L'\n') {
      if (isBeginningOfLine)
        std::wcout.put(section_chead);
      else if (section_suffix)
        std::wcout << section_suffix;

      std::wcout.put(ch);
      isBeginningOfLine = true;
    } else {
      if (isBeginningOfLine) {
        isBeginningOfLine = false;
        std::wcout.put(section_chead);
        if (section_prefix) std::wcout << section_prefix;
      }
      putchar_escaped(ch);
    }
  }

  void section_reset(){
    if (!isBeginningOfLine) {
      std::wcout.put(L'\n');
      if (section_suffix) std::wcout << section_suffix;
    }

    isBeginningOfLine = true;
    section_chead = ' ';
    section_prefix = nullptr;
    section_suffix = nullptr;
  }

  void putword(std::wstring const& text, int& index, int until, const wchar_t* prefix = nullptr, const wchar_t* suffix = nullptr) {
    if (index < until) {
      using std::swap;
      if (prefix) std::wcout << prefix;
      swap(section_prefix, prefix);
      swap(section_suffix, suffix);

      while (index < until) {
        wchar_t const ch = text[index++];
        section_putchar(ch);
      }

      swap(section_prefix, prefix);
      swap(section_suffix, suffix);
      if (suffix) std::wcout << suffix;
    }
  }

  void output_detailed_diff(std::wstring const& removed, std::wstring const& added) {
    std::vector<std::pair<int,int> > path;
    _ldiff_getpath_wu(path, removed, added);
    int index = 0;

    // removed
    index = 0;
    section_setup(L'-', termcap_begin_rline, termcap_end_rline);
    for (auto const& spec: path) {
      putword(removed, index, spec.first, termcap_begin_rword, termcap_end_rword);

      while (index < removed.size() && removed[index] == added[spec.second - spec.first + index]) {
        section_putchar(removed[index++]);
      }
    }
    putword(removed, index, removed.size(), termcap_begin_rword, termcap_end_rword);
    section_reset();

    // add
    section_setup(L'+', termcap_begin_aline, termcap_end_aline);
    index = 0;
    for (auto const& spec: path) {
      putword(added, index, spec.second, termcap_begin_aword, termcap_end_aword);
      while (index < added.size() && added[index] == removed[spec.first - spec.second + index]) {
        section_putchar(added[index++]);
      }
    }
    putword(added, index, added.size(), termcap_begin_aword, termcap_end_aword);
    section_reset();
  }

  std::wstring removed;
  std::wstring added;

  void output_buffered_lines() {
    if (!removed.empty()) {
      if (!added.empty()) {
        output_detailed_diff(removed, added);
        added.clear();
      } else {
        section_setup(L'-', termcap_begin_rline, termcap_end_rline);
        for (wchar_t ch: removed) section_putchar(ch);
        section_reset();
      }

      removed.clear();
    }
  }

  void process() {
    std::wstring line;
    while (std::getline(std::wcin, line)) {

      if (line[0] == '-') {
        if (!added.empty()) output_buffered_lines();

        removed.append(line, 1, line.size() - 1);
        removed.push_back(L'\n');
      } else if (line[0] == '+') {
        if (!removed.empty()) {
          added.append(line, 1, line.size() - 1);
          added.push_back(L'\n');
        } else {
          std::wcout << termcap_begin_aline;
          for (wchar_t ch: line) putchar_escaped(ch);
          std::wcout << termcap_end_aline;
          std::wcout.put(L'\n');
        }
      } else {
        if (!removed.empty()) output_buffered_lines();

        for (wchar_t ch: line) putchar_escaped(ch);
        std::wcout.put(L'\n');
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
