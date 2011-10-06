
#include "Mahalo.h"
#include "Noiser.h"
#include <unistd.h>

float fun(int t){
  return (float)((t >> 6 | t | t >> (t>>16)) * 10 + ((t>>11)&7));
}

int main(int argc, char *argv[]){

  Mahalo *m = new Mahalo();
  m->setup();

  Noiser *n = new Noiser(fun);
  m->setSampleSource(n); 
  
  m->sstart();

  usleep(5000000);

  m->sstop();
}
