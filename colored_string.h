// -*- mode:C++ -*-
#ifndef COLORED_STRING_H
#define COLORED_STRING_H
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>

typedef unsigned short word;
typedef unsigned char byte;

struct cc {
  static const byte def     = 0xff;

  static const byte gray    = 010;
  static const byte red     = 011;
  static const byte green   = 012;
  static const byte blue    = 014;
  static const byte cyan    = 016;
  static const byte magenta = 015;
  static const byte yellow  = 013;
  static const byte white   = 017;

  static const byte black   = 000;
  static const byte darkR   = 001;
  static const byte darkG   = 002;
  static const byte darkB   = 004;
  static const byte darkC   = 006;
  static const byte darkM   = 005;
  static const byte darkY   = 003;
  static const byte silver  = 007;

  static const word cc_default = def|def << 8;
  static short cc_overwrite(word c1, word c2) {
    bool w_fc = (c2 & 0xFF) != cc::def;
    bool w_bc = (c2 >> 8 & 0xFF) != cc::def;
    return (w_fc ? c2 : c1) & 0x00FF | (w_bc ? c2 : c1) & 0xFF00;
  }
public:
  static void set_color(word c);
public:
  static void clear();
};

class colored_string {
public:
  word color;
  word* colors;
  std::string str;
public:
  colored_string(const std::string& str): str(str), colors(NULL) {
    this->alloc_colors(str.size());
    color = cc::cc_default;
  }
  ~colored_string() {
    delete[] this->colors;
  }
private:
  void alloc_colors(int size) {
    if (this->colors)
      delete[] this->colors;

    this->colors = new word[size];
    for (int i = 0; i < size; i++)
      colors[i] = cc::cc_default;
  }
public:
  char& operator[](int index) {
    return str[index];
  }
  char operator[](int index) const {
    return str[index];
  }
  int size() const { return str.size(); }
  const char* c_str() const { return str.c_str(); }
  colored_string& operator=(const char* str) {
    int oldsize = this->str.size();
    word* oldcolors = this->colors;
    this->colors = NULL;

    this->str = str;
    int newsize = this->str.size();
    this->alloc_colors(newsize);
    std::memcpy(this->colors, oldcolors, sizeof(word) * std::min(oldsize, newsize));

    delete[] oldcolors;
  }
public:
  void set_fc(byte c){
    color = color & 0xFF00 | c;
  }
  void set_fc(byte c,int index){
    colors[index] = colors[index] & 0xFF00 | c;
  }
  void set_bc(byte c) {
    color = color & 0x00FF | c << 8;
  }
  void set_bc(byte c, int index) {
    colors[index] = colors[index] & 0x00FF | c << 8;
  }
  void set(word c) { color = c; }
  void set(word c, int index) { colors[index] = c; }
  word get() const { return color; }
  word get(int index) const { return colors[index]; }
public:
  void print() {
    if (str.size() == 0) return;

    word c;
    cc::set_color(c = this->color);
    for (int i = 0, iM = str.size(); i < iM; i++) {
      // determine character color
      word ic=cc::cc_overwrite(color, colors[i]);
      //printf("%04x<%04x=%04x\n", color, colors[i], ic);

      // set color
      if (c != ic) cc::set_color(c = ic);
      std::putchar(str[i]);
    }
    cc::clear();
  }
};
#endif
