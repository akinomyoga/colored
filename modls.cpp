#include <cstdlib>
#include <ctime>
#include <cstring>

#if 0
#define ESC(x) "\33[" #x "m"

int datenum(std::time_t t){
  std::tm* dt=std::localtime(&t);
  return dt->tm_year*10000+dt->tm_mon*100+dt->tm_mday;
}

class LineProc{
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
};

#endif

#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include "scanner.h"
#include "colored_string.h"
#include "filetype.h"
#include "argumentreader.h"

//******************************************************************************
//    Argument Reader
//------------------------------------------------------------------------------
class Argument:public mwg::ArgumentReader{
  typedef mwg::ArgumentReader base;
public:
  bool flag_time;
  std::vector<const char*> lines;
public:
  Argument():base("modls"){
    this->flag_time=false;
  }
  void print_usage(){
    std::printf("usage: modls [options]\n");
  }
  void arg_normal(const char* arg){
    lines.push_back(arg);
    //report_argerr("unrecognized argument");
  }
  bool switch_cflag(int letter){
    switch(letter){
    case 't':
      this->flag_time=true;
      return true;
    default:
      return false;
    }
  }
} args;

//******************************************************************************
//    read line
//------------------------------------------------------------------------------
struct read_data{
  std::string mods;
  std::string nlinks;
  std::string user;
  std::string space1;
  std::string group;
  std::string size;
  std::string time;
  std::string file;
  std::string note;
};

void read_line(read_data& data,const std::string line){
  line_scanner scanner(line);
  scanner.read(data.mods,"S");
  scanner.read(data.nlinks,"sSs");
  scanner.read(data.user,"S");
  scanner.read(data.space1,"s");
  scanner.read(data.group,"S");
  if(data.mods[0]=='b'||data.mods[0]=='c'){
    scanner.read(data.size,"sSsSs"); // number, number
  }else{
    scanner.read(data.size,"sSs");
  }
  scanner.read(data.time,args.flag_time?"Ss":"SsSs");

  /*
  scanner.read(data.file,"S");
  scanner.read(data.note,"*");
  /*/
  for(;;){
    std::string s;scanner.read(s,"s");
    std::string w;scanner.read(w,"S");
    if(w.size()==0)break;
    if(w!="->"){
      data.file+=s;
      data.file+=w;
    }else{
      data.note=s;
      data.note+=w;
      scanner.read(data.note,"*");
      break;
    }
  }
  //*/
}

//******************************************************************************
//    colored data
//------------------------------------------------------------------------------
struct line_data{
  colored_string mods;
  std::string nlinks;
  colored_string user;
  std::string space1;
  std::string group;
  std::string size;
  colored_string time;
  colored_string file;
  std::string note;
public:
  line_data(const read_data& data)
    :mods(data.mods)
    ,nlinks(data.nlinks)
    ,user(data.user)
    ,space1(data.space1)
    ,group(data.group)
    ,size(data.size)
    ,time(data.time)
    ,file(data.file)
    ,note(data.note)
  {}
public:
  void print(){
    mods.print();
    std::printf("%s",nlinks.c_str());
    user.print();
    std::printf("%s",space1.c_str());
    std::printf("%s",group.c_str());
    std::printf("%s",size.c_str());
    time.print();
    file.print();
    std::printf("%s",note.c_str());
    std::putchar('\n');
  }
};
//******************************************************************************
//    color
//------------------------------------------------------------------------------

ft::map_t extmap;

//------------------------------------------------------------------------------
void color_modifier(line_data& ldata){
  if(ldata.mods[0]=='d'){
    ldata.mods.set_fc(cc::blue,0);
  }else if(ldata.mods[0]=='l'){
    ldata.mods.set_fc(cc::cyan,0);
  }

  for(int i=1;i<=9;i++){
    switch(ldata.mods[i]){
    case 'r':ldata.mods.set_fc(cc::blue,i);break;
    case 'w':ldata.mods.set_fc(cc::red,i);break;
    case 'x':ldata.mods.set_fc(cc::green,i);break;
    case '-':ldata.mods.set_fc(cc::gray,i);break;
    case 't':
      ldata.mods.set_fc(cc::white,i);
      if(ldata.mods[i-1]=='w')
        ldata.mods.set_bc(cc::darkG,i);
      else
        ldata.mods.set_bc(cc::darkB,i);
      break;
    default:
      ldata.mods.set_fc(cc::white,i);
      ldata.mods.set_bc(cc::black,i);
      break;
    }
  }
}

//------------------------------------------------------------------------------
void color_filename(line_data& ldata){
  // filetype and color
  switch(ldata.mods[0]){
  case 'd':
    ldata.file.set_fc(cc::blue);
    ldata.mods.set_fc(cc::blue,0);
    return;
  case 'l':
    ldata.file.set_fc(cc::cyan);
    ldata.mods.set_fc(cc::cyan,0);
    return;
  case 'b': // block device
  case 'c': // character device
    ldata.file.set_bc(cc::black);
    ldata.file.set_fc(cc::yellow);
    ldata.mods.set_bc(cc::black,0);
    ldata.mods.set_fc(cc::yellow,0);
    return;
  case 's': // socket ?
    ldata.file.set_bc(cc::black);
    ldata.file.set_fc(cc::green);
    ldata.mods.set_bc(cc::black,0);
    ldata.mods.set_fc(cc::green,0);
    return;
  case 'p': // pipe (fifo)
    ldata.file.set_bc(cc::black);
    ldata.file.set_fc(cc::cyan);
    ldata.mods.set_bc(cc::black,0);
    ldata.mods.set_fc(cc::cyan,0);
    return;
  }

  if(ldata.mods[9]=='t'){
    ldata.file.set(ldata.mods.get(9));
    return;
  }else if(ldata.mods[3]=='x'){
    ldata.file.set_fc(cc::green);
    return;
  }

  // extension
  ft::map_t::iterator i=extmap.find(get_extension(ldata.file.str));
  if(i!=extmap.end())switch(i->second){
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
void color_time(line_data& ldata){
  if(args.flag_time){
    std::time_t time=(std::time_t)std::atol(ldata.time.str.c_str());

    // format time
    std::tm* dt=std::localtime(&time);
    char buff[128];
    std::sprintf(buff,"%04d-%02d-%02d %02d:%02d:%02d ",
      1900+dt->tm_year,1+dt->tm_mon,dt->tm_mday,
      dt->tm_hour,dt->tm_min,dt->tm_sec
      );
    ldata.time=buff;

    // color time
    std::time_t now=std::time(NULL);
    typedef unsigned int uint32t;
    float delta=(uint32t)now-(uint32t)time;
    if((delta/=60)<=1){
      ldata.time.set_fc(cc::red);
    }else if(delta<=5){
      ldata.time.set_fc(cc::cyan);
    }else if(delta<=15){
      ldata.time.set_fc(cc::green);
    }else if((delta/=60)<=1){
      ldata.time.set_fc(cc::blue);
    }else if(delta<=6){
      ldata.time.set_fc(cc::darkC);
    }else if((delta/=24)<=1){
      ldata.time.set_fc(cc::darkB);
    }else if(delta<=7){
      ldata.time.set_fc(cc::darkM);
    }else if(delta<=30){
      ldata.time.set_fc(cc::darkY);
    }
  }
}
//******************************************************************************
// -rwxr-xr-x  1 k-murase jlc   74 Nov 11 15:27 escape-regex
void process_line(const std::string line){
  read_data rdata;
  read_line(rdata,line);

  line_data ldata(rdata);
  if(ldata.mods.size()!=10&&ldata.mods.size()!=11){
    puts(line.c_str());
    return;
  }

  // color mods
  color_modifier(ldata);
  color_filename(ldata);
  color_time(ldata);

  ldata.print();
}

int main(int argc,char** argv){
  if(!args.read(argc,argv))return 1;

  if(args.lines.size()>0){
    typedef std::vector<const char*>::iterator it;
    for(it i=args.lines.begin();i!=args.lines.end();i++){
      process_line(*i);
    }
    return 0;
  }

  ft::init_map(extmap);

  std::string line;
  while(std::getline(std::cin,line)){
    process_line(line);
  }
  return 0;
}
