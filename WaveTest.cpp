
#include "Mahalo.h"
#include "SampleMixer.h"
#include "Wave.h"
#include "tones.h"
#include <unistd.h>
int main(int argc, char *argv[]){

  Mahalo *m = new Mahalo();
  m->setup();

  Wave *a = new Wave(C4,m->getRate(),0.5); 
  Wave *b = new Wave(E4,m->getRate(),0.5); 
  Wave *c = new Wave(G4,m->getRate(),0.5); 
  
  SampleMixer *mix = new SampleMixer();
  mix->Add(a);
  mix->Add(b);
  mix->Add(c);
  
  m->setSampleSource(mix); 
  
  m->sstart();

  usleep(5000000);

  m->sstop();
}
