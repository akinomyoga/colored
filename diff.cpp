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

void puttext(const wchar_t* text) {
  while (*text) std::putwchar(*text++);
}
void putuntil(std::wstring const& text, int& index, int until, const wchar_t* prefix = nullptr, const wchar_t* suffix = nullptr) {
  if (index < until) {
    if (prefix) puttext(prefix);
    while (index < until)
      std::putwchar(text[index++]);
    if (suffix) puttext(suffix);
  }
}

int main() {
  std::setlocale(LC_CTYPE, "");
  std::ios_base::sync_with_stdio(false);
  std::locale::global(std::locale(std::locale::classic(), std::locale(""), std::locale::ctype));
  std::wcout.imbue(std::locale());
  std::wcin.imbue(std::locale());

  std::wstring line;
  std::wstring prev;
  while (std::getline(std::wcin, line)) {
    //std::wcout << line << "\n";
    std::vector<std::pair<int,int> > path;
    _ldiff_getpath_wu(path, prev, line);
    int index = 0;

    // removed
    index = 0;
    std::putwchar(L'-');
    for (auto const& spec: path) {
      putuntil(prev, index, spec.first, L"\x1b[1;48;5;217m", L"\x1b[m");

      while (index < prev.size() && prev[index] == line[spec.second - spec.first + index]) {
        std::putwchar(prev[index++]);
      }
    }
    putuntil(prev, index, prev.size(), L"\x1b[1;48;5;217m", L"\x1b[m");
    std::putwchar(L'\n');

    // add
    std::putwchar(L'+');
    index = 0;
    for (auto const& spec: path) {
      putuntil(line, index, spec.second, L"\x1b[1;48;5;155m", L"\x1b[m");
      while (index < line.size() && line[index] == prev[spec.first - spec.second + index]) {
        std::putwchar(line[index++]);
      }
    }
    putuntil(line, index, line.size(), L"\x1b[1;48;5;155m", L"\x1b[m");
    std::putwchar(L'\n');

    prev = line;
  }

  std::wcout << std::flush;
}
