#ifndef COLORED_STRING_H
#define COLORED_STRING_H
#include <cstdlib>
#include <cstdio>
#include <string>

typedef unsigned short word;
typedef unsigned char byte;

struct cc{
  static const byte def    =0xff;

  static const byte gray   =0x10;
  static const byte red    =0x11;
  static const byte green  =0x12;
  static const byte blue   =0x14;
  static const byte cyan   =0x16;
  static const byte magenta=0x15;
  static const byte yellow =0x13;
  static const byte white  =0x17;

  static const byte black  =0x00;
  static const byte darkR  =0x01;
  static const byte darkG  =0x02;
  static const byte darkB  =0x04;
  static const byte darkC  =0x06;
  static const byte darkM  =0x05;
  static const byte darkY  =0x03;
  static const byte silver =0x07;

  static const word cc_default=def|def<<8;
  static short cc_overwrite(word c1,word c2){
    bool w_fc=(c2&0xFF)!=cc::def;
    bool w_bc=(c2>>8&0xFF)!=cc::def;
    return (w_fc?c2:c1)&0x00FF|(w_bc?c2:c1)&0xFF00;
  }
public:
  static void set_color(word c){
    cc::clear();
    if(c==cc::cc_default)return;
    byte fc=c&0xff;
    byte bc=c>>8&0xff;
    std::putchar('\33');
    std::putchar('[');
    if(fc!=cc::def)put_forecolor(fc);
    if(fc!=cc::def&&bc!=cc::def)std::putchar(';');
    if(bc!=cc::def)put_backcolor(bc);
    std::putchar('m');
  }
  static void clear(){
    std::putchar('\33');
    std::putchar('[');
    std::putchar('m');
  }
private:
  static void put_forecolor(byte c){
    if(c&0x10){
      std::putchar('1');
      std::putchar(';');
    }
    std::putchar('3');
    std::putchar('0'+(c&0x7));
  }
  static void put_backcolor(byte c){
    if(c&0x10){
      std::putchar('5');
      std::putchar(';');
    }
    std::putchar('4');
    std::putchar('0'+(c&0x7));
  }
};

class colored_string{
public:
  word color;
  word* colors;
  std::string str;
public:
  colored_string(const std::string& str):str(str){
    this->colors=new word[str.size()];
    for(int i=0,iM=str.size();i<iM;i++)
      colors[i]=cc::cc_default;
    color=cc::cc_default;
  }
  ~colored_string(){
    delete[] this->colors;
  }
public:
  char& operator[](int index){
    return str[index];
  }
  char operator[](int index) const{
    return str[index];
  }
  int size() const{return str.size();}
public:
  void set_fc(byte c){
    color=color&0xFF00|c;
  }
  void set_fc(byte c,int index){
    colors[index]=colors[index]&0xFF00|c;
  }
  void set_bc(byte c){
    color=color&0x00FF|c<<8;
  }
  void set_bc(byte c,int index){
    colors[index]=colors[index]&0x00FF|c<<8;
  }
public:
  void print(){
    word c;
    cc::set_color(c=this->color);
    for(int i=0,iM=str.size();i<iM;i++){
      // determine character color
      word ic=cc::cc_overwrite(color,colors[i]);
      //printf("%04x<%04x=%04x\n",color,colors[i],ic);

      // set color
      if(c!=ic)cc::set_color(c=ic);
      std::putchar(str[i]);
    }
    cc::clear();
  }
};
#endif