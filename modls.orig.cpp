#include <cstdlib>
#include <cstdio>
#include <string>
#include <iostream>

#define ESC(x) "\33[" #x "m"

int datenum(time_t t){
  tm* dt=localtime(&t);
  return dt->tm_year*10000+dt->tm_mon*100+dt->tm_mday;
}

std::string get_extension(const std::string& filename){
  const char* sfilename=filename.c_str();
  int i=filename.find_last_of("./");
  if(i>=0&&sfilename[i]=='.'){
    const char* ext=sfilename+i;
    int len=filename.length()-i;
    if(len>4&&strcmp("\33[0m",ext+len-4)==0)len-=4;
    return std::string(ext,len);
  }else{
    return "";
  }
}

std::string set_color(const std::string& text,int coloridx,bool highlight=false){
  const char* str=text.c_str();
  int len=text.length();

  // cut default escapes
  if(len>2&&str[0]=='\33'&&str[1]=='['){
    while(len>0){
      char c=*str++;len--;
      if('a'<=c&&c<='z'||'A'<=c&&c<='Z')break;
    }
  }

  if(len>=4&&strcmp(str+len-4,"\33[0m")==0){
    len-=4;
  }

  char head[32];
  sprintf(head,"\33[%s%dm",highlight?"1;":"",coloridx);

  // decorate
  std::string ret=head;
  ret.append(str,len);
  ret+="\33[0m";

  return ret;
}

class LineProc{
public:
  int i;
  const char* s;
  char c;
  std::string buff;
  std::string mod;
  std::string s1;
  std::string lnk;
  std::string s2;
  std::string usr;
  std::string s3;
  std::string jlc;
  std::string s4;
  std::string sz;
  std::string s5;
  std::string dt;
  std::string s6;
  std::string fn;

  std::string sec;
  LineProc(const char* line):i(0),s(line),c('^'){
    next();

    read();
    write();
  }
private:
  bool next(){
    if(c=='\0')return false;
    c=s[i++];
    return c!='\0';
  }
private:
  void read(){
    read_w(mod);
    read_s(s1);

    read_w(lnk);
    read_s(s2);

    read_w(usr);
    read_s(s3);

    read_w(jlc);
    read_s(s4);

    read_w(sz);
    read_s(s5);

    read_w(sec);
    read_s(sec);

    read_w(dt);
    read_s(dt);
    read_w(dt);
    read_s(s6);

    read_r(fn);
  }
  void read_w(std::string& x){
    while(c&&c!=' '){
      x+=c;next();
    }
  }
  void read_s(std::string& x){
    while(c&&c==' '){
      x+=c;next();
    }
  }
  void read_r(std::string& x){
    while(c){
      x+=c;next();
    }
  }
private:
  void color_mod(){
    //mod=ESC(31)+mod+ESC(0);
  }
  void color_lnk(){
    //printf("%x ",mod[3]);
    if(mod.find("total")>=0||lnk=="1")return;
    lnk=ESC(1;36)+lnk+ESC(0);
  }
  void color_usr(){
    if(usr=="k-murase")return;
    usr=ESC(34)+usr+ESC(0);
  }
  void color_dt(){
    time_t ct=time(NULL);
    time_t ft=atol(sec.c_str());
    int cd=datenum(ct);
    int fd=datenum(ft);

    int i=ct-ft;
    if(i<60){
      dt=ESC(1;32)+dt+ESC(0);
    }else if(i<300){
      dt=ESC(1;36)+dt+ESC(0);
    }else if(i<900){
      dt=ESC(1;34)+dt+ESC(0);
    }else if(cd==fd){
      dt=ESC(0;35)+dt+ESC(0);
    }
  }
  void color_fn(){
    std::string ext=get_extension(fn);
    if(ext==".gif"||ext==".eps"){
      fn=set_color(fn,35);
    }else{
      //fn=fn+" (ext == "+ext+")";
    }
  }
private:
  void write(){
    color_mod();
    color_lnk();
    color_usr();
    color_dt();
    color_fn();
    buff+=mod;
    buff+=s1;
    buff+=lnk;
    buff+=s2;
    buff+=usr;
    buff+=s3;
    buff+=jlc;
    buff+=s4;
    buff+=sz;
    buff+=s5;
    buff+=dt;
    buff+=s6;
    buff+=fn;

    std::cout<<buff<<std::endl;
  }
};


// -rwxr-xr-x  1 k-murase jlc   74 Nov 11 15:27 escape-regex
int main(int argc,char** argv){
  if(argc>1){
    LineProc m(argv[1]);
    return 0;
  }

  std::string line;
  while(std::getline(std::cin,line)){
    LineProc m(line.c_str());
  }
  return 0;
}

