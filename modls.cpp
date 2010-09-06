#include <cstdlib>
#include <ctime>
#include <cstring>

#if 0
#define ESC(x) "\33[" #x "m"

int datenum(time_t t){
  tm* dt=localtime(&t);
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
#include "scanner.h"
#include "colored_string.h"
#include "filetype.h"
//------------------------------------------------------------------------------
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
  scanner.read(data.time,"SsSs");
  scanner.read(data.file,"S");
  scanner.read(data.note,"*");
}

//------------------------------------------------------------------------------
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
//------------------------------------------------------------------------------
//    color
//------------------------------------------------------------------------------

ft::map_t extmap;

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

  color_filename(ldata);

  ldata.print();
}

int main(int argc,char** argv){
  if(argc>1){
    process_line(argv[1]);
    return 0;
  }

  ft::init_map(extmap);

  std::string line;
  while(std::getline(std::cin,line)){
    process_line(line);
  }
  return 0;
}
