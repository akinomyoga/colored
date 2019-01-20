#include <cstdlib>
#include <ctime>
#include <cstring>

#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include "scanner.h"
#include "colored_string.h"
#include "filetype.h"
#include "argumentreader.h"

#if defined(__unix) || defined(__unix__)
# define mwg_modls_unistd_available
# include <sys/types.h>
# include <sys/stat.h>
# include <unistd.h>
#endif

//******************************************************************************
//    Argument Reader
//------------------------------------------------------------------------------
class params_t: public mwg::ArgumentReader {
  typedef mwg::ArgumentReader base;
public:
  bool flag_time;
  bool flag_user;
  bool flag_group;
  bool flag_context;
  std::vector<const char*> lines;
public:
  params_t(): base("modls") {
    this->flag_time = false;
    this->flag_user = true;
    this->flag_group = true;
    this->flag_context = false;
  }
  void print_usage() {
    std::printf("usage: modls [options]\n");
  }
  void arg_normal(const char* arg) {
    lines.push_back(arg);
    //report_argerr("unrecognized argument");
  }
  bool switch_cflag(int letter) {
    switch (letter) {
    case 't':
      this->flag_time = true;
      return true;
    case 'U':
      this->flag_user = false;
      return true;
    case 'G':
      this->flag_group = false;
      return true;
    case 'Z':
      this->flag_context = true;
      return true;
    default:
      return false;
    }
  }
} args;

//******************************************************************************
//    read line
//------------------------------------------------------------------------------
struct read_data {
  std::string mods;
  std::string nlinks;
  std::string user;

  std::string space1;
  std::string group;
  std::string space2;
  std::string se_context;
  std::string space3;
  std::string size;
  std::string time;
  std::string file;
  std::string note;
  std::string link;
};

void read_line(read_data& data, const std::string& line) {
  line_scanner scanner(line);
  scanner.read(data.mods, "S");
  scanner.read(data.nlinks, "sSs");
  if (args.flag_user) {
    scanner.read(data.user, "S");
    scanner.read(data.space1, "s");
  }
  if (args.flag_group) {
    scanner.read(data.group, "S");
    scanner.read(data.space2, "s");
  }
  if (args.flag_context) {
    scanner.read(data.se_context, "S");
    scanner.read(data.space3, "s");
  }
  if (data.mods[0] == 'b' || data.mods[0] == 'c') {
    scanner.read(data.size, "SsSs"); // number, number
  } else {
    scanner.read(data.size, "Ss");
  }
  scanner.read(data.time, args.flag_time ? "Ss" : "SsSs");

  /*
  scanner.read(data.file,"S");
  scanner.read(data.note,"*");
  /*/
  for (;;) {
    std::string s; scanner.read(s, "s");
    std::string w; scanner.read(w, "S");
    if (w.size() == 0) break;
    if (w != "->") {
      data.file += s;
      data.file += w;
    } else {
      // data.note=" -> "
      data.note = s;
      data.note += w;
      s.clear();
      scanner.read(s, "s");
      data.note += s;

      // data.link
      scanner.read(data.link, "*");
      break;
    }
  }
  //*/
}

//******************************************************************************
//    colored data
//------------------------------------------------------------------------------
struct line_data {
  colored_string mods;
  std::string nlinks;
  colored_string user;
  std::string space1;
  std::string group;
  std::string space2;
  std::string se_context;
  std::string space3;
  std::string size;
  colored_string time;
  colored_string file;
  std::string note;
  colored_string link;
public:
  line_data(const read_data& data):
    mods(data.mods),
    nlinks(data.nlinks),
    user(data.user),
    space1(data.space1),
    group(data.group),
    space2(data.space2),
    se_context(data.se_context),
    space3(data.space3),
    size(data.size),
    time(data.time),
    file(data.file),
    note(data.note),
    link(data.link)
  {}
public:
  void print() {
    mods.print();
    std::printf("%s", nlinks.c_str());
    if (args.flag_user) {
      user.print();
      std::printf("%s", space1.c_str());
    }
    if (args.flag_group)
      std::printf("%s%s", group.c_str(), space2.c_str());
    if (args.flag_context)
      std::printf("%s%s", se_context.c_str(), space3.c_str());
    std::printf("%s", size.c_str());
    time.print();
    std::putchar(' ');
    file.print();
    std::printf("%s", note.c_str());
    link.print();
    std::putchar('\n');
  }
};
//******************************************************************************
//    color
//------------------------------------------------------------------------------

ft::map_t extmap;

//------------------------------------------------------------------------------
void color_modifier(line_data& ldata) {
  if (ldata.mods[0] == 'd') {
    ldata.mods.set_fc(cc::blue, 0);
  } else if (ldata.mods[0] == 'l') {
    ldata.mods.set_fc(cc::cyan, 0);
  }

  for (int i = 1; i <= 9; i++) {
    switch (ldata.mods[i]) {
    case 'r': ldata.mods.set_fc(cc::blue, i); break;
    case 'w': ldata.mods.set_fc(cc::red, i); break;
    case 'x': ldata.mods.set_fc(cc::green, i); break;
    case '-': ldata.mods.set_fc(cc::gray, i); break;
    case 't':
      ldata.mods.set_fc(cc::white, i);
      if (ldata.mods[i - 1] == 'w')
        ldata.mods.set_bc(cc::darkG, i);
      else
        ldata.mods.set_bc(cc::darkB, i);
      break;
    default:
      ldata.mods.set_fc(cc::white, i);
      ldata.mods.set_bc(cc::black, i);
      break;
    }
  }
}

//------------------------------------------------------------------------------
void color_filename(line_data& ldata) {
  // filetype and color
  switch (ldata.mods[0]) {
  case 'd':
    ldata.file.set_fc(cc::blue);
    ldata.mods.set_fc(cc::blue, 0);
    return;
  case 'l':
    ldata.file.set_fc(cc::cyan);
    ldata.mods.set_fc(cc::cyan, 0);
    return;
  case 'b': // block device
  case 'c': // character device
    ldata.file.set_bc(cc::black);
    ldata.file.set_fc(cc::yellow);
    ldata.mods.set_bc(cc::black, 0);
    ldata.mods.set_fc(cc::yellow, 0);
    return;
  case 's': // socket ?
    ldata.file.set_bc(cc::black);
    ldata.file.set_fc(cc::green);
    ldata.mods.set_bc(cc::black, 0);
    ldata.mods.set_fc(cc::green, 0);
    return;
  case 'p': // pipe (fifo)
    ldata.file.set_bc(cc::black);
    ldata.file.set_fc(cc::cyan);
    ldata.mods.set_bc(cc::black, 0);
    ldata.mods.set_fc(cc::cyan, 0);
    return;
  }

  if (ldata.mods[9] == 't') {
    ldata.file.set(ldata.mods.get(9));
    return;
  } else if(ldata.mods[3] == 'x') {
    ldata.file.set_fc(cc::green);
    return;
  }

  // extension
  ft::map_t::iterator i = extmap.find(get_extension(ldata.file.str));
  if (i != extmap.end()) switch(i->second) {
  case ft::image:
    ldata.file.set_fc(cc::magenta);
    return;
  case ft::archive:
    ldata.file.set_fc(cc::red);
    return;
  case ft::media:
    ldata.file.set_fc(cc::darkC);
    return;
  }
}
//------------------------------------------------------------------------------
void color_time(line_data& ldata) {
  if (args.flag_time) {
    std::time_t time = (std::time_t) std::atol(ldata.time.str.c_str());

    // format time
    std::tm* dt = std::localtime(&time);
    char buff[128];
    std::sprintf(buff, "%04d-%02d-%02d %02d:%02d:%02d",
      1900 + dt->tm_year, 1 + dt->tm_mon, dt->tm_mday,
      dt->tm_hour, dt->tm_min, dt->tm_sec);
    ldata.time = buff;

    // color time
    std::time_t now = std::time(NULL);
    float delta = (float) now - (float) time;
    if (delta < 0) {
      ldata.time.set_fc(cc::yellow);
      ldata.time.set_bc(cc::black);
    } else if ((delta /= 60) <= 1) {
      ldata.time.set_fc(cc::red);
    } else if (delta <= 5) {
      ldata.time.set_fc(cc::cyan);
    } else if (delta <= 15) {
      ldata.time.set_fc(cc::green);
    } else if ((delta /= 60) <= 1) {
      ldata.time.set_fc(cc::blue);
    } else if (delta <= 6) {
      ldata.time.set_fc(cc::darkC);
    } else if ((delta /= 24) <= 1) {
      ldata.time.set_fc(cc::darkB);
    } else if (delta <= 7) {
      ldata.time.set_fc(cc::darkM);
    } else if (delta <= 30) {
      ldata.time.set_fc(cc::darkY);
    }
  }
}
//------------------------------------------------------------------------------
void color_link(line_data& ldata) {
  if (ldata.mods[0] != 'l') return;

#ifdef mwg_modls_unistd_available
  //■ディレクトリのメンバに対しては ldata.file の部分に表示されるのは単なるファイル名。
  char buff[1000];
  ssize_t len = readlink(ldata.file.c_str(), buff, sizeof(buff) - 1);
  if (0 <= len && (std::size_t) len < sizeof(buff)) {
    buff[len] = '\0';
    struct stat st;
    if (::stat(buff, &st) == 0) {
      if (S_ISDIR(st.st_mode)) { // d
        ldata.link.set_fc(cc::blue);
        return;
      } else if (S_ISLNK(st.st_mode)) { // h
        ldata.link.set_fc(cc::cyan);
        return;
      } else if (S_ISFIFO(st.st_mode)) { // p
        ldata.link.set_bc(cc::black);
        ldata.link.set_fc(cc::cyan);
        return;
      } else if (S_ISCHR(st.st_mode) || S_ISBLK(st.st_mode)) { // b c
        ldata.link.set_bc(cc::black);
        ldata.link.set_fc(cc::yellow);
        return;
      } else if (S_ISSOCK(st.st_mode)) { // s
        ldata.link.set_bc(cc::black);
        ldata.link.set_fc(cc::green);
        return;
      } else if (S_ISVTX & st.st_mode) { // t
        ldata.link.set_fc(cc::white);
        if (S_IWOTH & st.st_mode) // o+w 他人書込可能
          ldata.link.set_bc(cc::darkG);
        else
          ldata.link.set_bc(cc::darkB);
        return;
      } else if ((S_IXUSR | S_IXGRP | S_IXOTH) & st.st_mode) { // x
        ldata.link.set_fc(cc::green);
        return;
      }
    } else {
      // error
      ldata.link.set_fc(cc::white);
      ldata.link.set_bc(cc::red);
      return;
    }
  }
#endif

  // extension
  ft::map_t::iterator i = extmap.find(get_extension(ldata.link.str));
  if (i != extmap.end()) {
    switch (i->second) {
    case ft::image:
      ldata.link.set_fc(cc::magenta);
      return;
    case ft::archive:
      ldata.link.set_fc(cc::red);
      return;
    case ft::media:
      ldata.link.set_fc(cc::darkC);
      return;
    }
  }
}
//******************************************************************************
// -rwxr-xr-x  1 k-murase jlc   74 Nov 11 15:27 escape-regex
void process_line(const std::string& line) {
  read_data rdata;
  read_line(rdata, line);

  line_data ldata(rdata);
  if (ldata.mods.size() != 10 && ldata.mods.size() != 11) {
    puts(line.c_str());
    return;
  }

  // color mods
  color_modifier(ldata);
  color_filename(ldata);
  color_time(ldata);
  color_link(ldata);

  ldata.print();
}

int main(int argc, char** argv) {
  if (!args.read(argc, argv)) return 1;

  if (args.lines.size() > 0) {
    typedef std::vector<const char*>::iterator it;
    for (it i = args.lines.begin(); i != args.lines.end(); i++) {
      process_line(*i);
    }
    return 0;
  }

  ft::init_map(extmap);

#ifdef USE_TERMINFO
  void init_termcolor();
  init_termcolor();
#endif

  std::string line;
  while (std::getline(std::cin, line)) {
    process_line(line);
  }
  return 0;
}
