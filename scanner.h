#ifndef MODLS_SCANNER_H
#define MODLS_SCANNER_H
#include <cstdlib>
#include <string>
#include <cstring>
#include <cctype>

class line_scanner{
  const std::string s;
  int i;
  char c;
private:
  bool next(){
    if(c=='\0')return false;
    c=s[i++];
    return c!='\0';
  }
public:
  line_scanner(const std::string& s):s(s),i(0),c('^'){
    next();
  }
private:
  static bool pred_s(char c){return std::isspace(c);}
  static bool pred_S(char c){return !std::isspace(c);}
  static bool pred_d(char c){return std::isdigit(c);}
  static bool pred_D(char c){return !std::isdigit(c);}
  static bool pred_any(char c){return true;}
public:
  void read(std::string& buff,const char* fmt){
    for(int f=0,fM=std::strlen(fmt);f<fM;f++){
      // choose predicator from fmt
      bool (*pred)(char)=NULL;
      //debug:printf("choose predicator for %c\n",fmt[f]);
      switch(fmt[f]){
        case 's':pred=&line_scanner::pred_s;break;
        case 'S':pred=&line_scanner::pred_S;break;
        case '*':pred=&line_scanner::pred_any;break;
        default:continue;
      }

      while(c&&pred(c)){
        buff+=c;next();
      }
    }
  }
};
#endif
