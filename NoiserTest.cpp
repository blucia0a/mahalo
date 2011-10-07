
#include "Mahalo.h"
#include "Noiser.h"
#include <unistd.h>

float fun(int t){
  int val = (((t>>(t>>16)))*t) | (t>>7) ^ (t-1);
  unsigned char vc = (unsigned char)val;
  int retval = (int)vc;
  return (float)retval;
}

int main(int argc, char *argv[]){

  Mahalo *m = new Mahalo();
  m->setup();

  Noiser *n = new Noiser(fun);
  m->setSampleSource(n); 
  
  m->sstart();


  while(1); 


  m->sstop();
}
