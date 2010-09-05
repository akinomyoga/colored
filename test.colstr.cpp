#include "colored_string.h"

int main(){
  colored_string sss("Hello rgb! test\n");
  sss.set_fc(cc::black);
  sss.set_bc(cc::silver);
  sss.set_fc(cc::red,6);
  sss.set_fc(cc::green,7);
  sss.set_fc(cc::blue,8);
  sss.print();
  return 0;
}
