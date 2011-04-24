#ifndef MWG_ARGUMENTREADER
#define MWG_ARGUMENTREADER
#include <cstdio>
#include <cstdarg>
#include <cstring>
namespace mwg{

class ArgumentReader{
  const char* progname;
  int argc;
  char** argv;
  bool err;
  int i;
  int option;
  char* optionname;

  char buff[128];
public:
  ArgumentReader(const char* progname):progname(progname){}
protected:
  const char* format(const char* fmt,...){
    va_list ap;
    va_start(ap, fmt);
    std::vsprintf(buff,fmt,ap);
    va_end(ap);
    return buff;
  }
  void report_argerr(const char* message){
    std::printf("%s:(arg%d)! %s (%s)\n",progname,i,message,argv[i]);
    this->err=true;
  }
  void report_error(const char* name,const char* message){
    std::printf("%s:%s! %s\n",progname,name,message);
    this->err=true;
  }
protected:
  virtual void print_usage()=0;
  virtual void arg_normal(const char* arg)=0;
  virtual void last_check(){}
  virtual bool switch_cflag(int c){return false;}
  virtual bool is_cfunc(int c){return false;}
  virtual void switch_cfunc(int c,const char* arg){}
  virtual int resolve_longname(const char* arg){return 0;}
private:
  // [return: éwíËÇ≥ÇÍÇΩà¯êîÇÃì«Ç›éÊÇËÇ™èIóπÇµÇΩÇ©Ç«Ç§Ç©]
  bool read_option(char*& arg){
    int c=*arg++;
    if(c=='\0'){
      report_argerr("incomplete option");
      return true;
    }

    if(c=='-'&&*arg!='\0'){
      // --longname=param ï™ó£
      char* p1=std::strchr(arg,'=');
      char* p2=std::strchr(arg,':');
      if(p1==NULL||p2!=NULL&&p2<p1)p1=p2;
      if(p1!=NULL)*p1++='\0';

      // longname
      c=resolve_longname(arg);
      if(c==0){
        report_argerr(format("unrecongnized longname option '%s'",arg));
        return true;
      }

      if(switch_cflag(c)){
        if(p1)report_argerr(format("option '%s' requires no extra parameters",arg));
        return true;
      }

      if(is_cfunc(c)){
        option=c;
        optionname=arg;
        if(!p1)return true;
        arg=p1;
        return false;
      }
    }else{
      // flag
      while(switch_cflag(c)){
        if((c=*arg++)=='\0')return true;
      }

      // func
      if(is_cfunc(c)){
        option=c;
        if(*arg=='\0')return true;
        if(*arg==':'||*arg=='=')arg++;
        return false;
      }
    }

    // ???
    report_argerr(format("unrecognized option '%c'",c));
    return true;
  }
public:
  bool read(int argc,char** argv){
    // init
    this->argc=argc;
    this->argv=argv;
    this->err=false;

    option=0;
    optionname=NULL;
    for(i=1;i<argc;i++){
      char* arg=argv[i];
      if(!option){
        if(arg[0]=='\0')continue;
        if(arg[0]=='-'){
          arg++;
          if(read_option(arg))continue;
        }
      }

      if(option){
        switch_cfunc(option,arg);
      }else{
        arg_normal(arg);
      }

      option=0;
      optionname=NULL;
    }

    // check
    if(option!=0){
      if(optionname)
        report_argerr(format("missing extra-argument for option '%s'",optionname));
      else
        report_argerr(format("missing extra-argument for option '%c'",(char)option));
    }
    last_check();

    if(err)print_usage();
    return !err;
  }
};

}
#endif
