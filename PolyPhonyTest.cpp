#include "Mahalo.h"
#include "SampleMixer.h"
#include "Wave.h"
#include "tones.h"
#include <unistd.h>
#include <pthread.h>

void *longStrike(void *t){

  Wave *w = (Wave *)t;
  for(int i = 0; i < 1000; i++){
    usleep(100);
    w->setAmpVal(w->getAmpVal() + .001);
  }

  usleep(100000); 

  for(int i = 0; i < 1000; i++){
    usleep(1000);
    w->setAmpVal(w->getAmpVal() - .001);
  }

}

void *shortStrike(void *t){

  Wave *w = (Wave *)t;
  for(int i = 0; i < 100; i++){
    usleep(500);
    w->setAmpVal(w->getAmpVal() + .01);
  }

  for(int i = 0; i < 100; i++){
    usleep(100);
    w->setAmpVal(w->getAmpVal() - .01);
  }

  for(int i = 0; i < 100; i++){
    usleep(500);
    w->setAmpVal(w->getAmpVal() + .01);
  }

  usleep(200000); 

  for(int i = 0; i < 1000; i++){
    usleep(300);
    w->setAmpVal(w->getAmpVal() - .001);
  }
  w->setAmpVal(0);

}

pthread_t *strikeTone(Wave *w, void *(*shape)(void*)){
  
  pthread_t * t = (pthread_t*)malloc(sizeof(pthread_t));
  pthread_create(t,NULL,shape,(void*)w);
  return t;

}

int main(int argc, char *argv[]){

  Mahalo *m = new Mahalo();
  m->setup();

  SampleMixer *mix1 = new SampleMixer();

  Wave *keys[8];
  keys[0] = new Wave(C4,m->getRate(),0); 
  keys[1] = new Wave(D4,m->getRate(),0); 
  keys[2] = new Wave(E4,m->getRate(),0); 
  keys[3] = new Wave(F4,m->getRate(),0); 
  keys[4] = new Wave(G4,m->getRate(),0); 
  keys[5] = new Wave(A4,m->getRate(),0); 
  keys[6] = new Wave(B4,m->getRate(),0); 
  keys[7] = new Wave(C5,m->getRate(),0); 
 
  pthread_t *keyts[8];

  for(int i = 0; i < 8; i++){
    mix1->Add(keys[i]);
  }

  m->setSampleSource(mix1); 

  m->sstart();
  
  pthread_t *t = strikeTone(keys[3],shortStrike);
  pthread_join(*t,NULL);
  free(t);
  t = strikeTone(keys[5],shortStrike);
  pthread_join(*t,NULL);
  free(t);
  t = strikeTone(keys[7],shortStrike);
  pthread_join(*t,NULL);
  free(t);

  pthread_t *pt = strikeTone(keys[3],longStrike);
  pthread_t *pt2 = strikeTone(keys[5],longStrike);
  pthread_t *pt3 = strikeTone(keys[7],longStrike);
  pthread_join(*pt,NULL);
  pthread_join(*pt2,NULL);
  pthread_join(*pt3,NULL);
  free(pt);
  free(pt2);
  free(pt3);
  
  t = strikeTone(keys[2],shortStrike);
  pthread_join(*t,NULL);
  free(t);
  t = strikeTone(keys[4],shortStrike);
  pthread_join(*t,NULL);
  free(t);
  t = strikeTone(keys[6],shortStrike);
  pthread_join(*t,NULL);
  free(t);
  
  pt = strikeTone(keys[2],longStrike);
  pt2 = strikeTone(keys[4],longStrike);
  pt3 = strikeTone(keys[6],longStrike);
  pthread_join(*pt,NULL);
  pthread_join(*pt2,NULL);
  pthread_join(*pt3,NULL);
  free(pt);
  free(pt2);
  free(pt3);
  
  t = strikeTone(keys[1],shortStrike);
  pthread_join(*t,NULL);
  free(t);
  t = strikeTone(keys[3],shortStrike);
  pthread_join(*t,NULL);
  free(t);
  t = strikeTone(keys[5],shortStrike);
  pthread_join(*t,NULL);
  free(t);
  
  pt = strikeTone(keys[1],longStrike);
  pt2 = strikeTone(keys[3],longStrike);
  pt3 = strikeTone(keys[5],longStrike);
  pthread_join(*pt,NULL);
  pthread_join(*pt2,NULL);
  pthread_join(*pt3,NULL);
  free(pt);
  free(pt2);
  free(pt3);
  
  t = strikeTone(keys[0],shortStrike);
  pthread_join(*t,NULL);
  free(t);
  t = strikeTone(keys[2],shortStrike);
  pthread_join(*t,NULL);
  free(t);
  t = strikeTone(keys[4],shortStrike);
  pthread_join(*t,NULL);
  free(t);
  
  pt = strikeTone(keys[0],longStrike);
  pt2 = strikeTone(keys[2],longStrike);
  pt3 = strikeTone(keys[4],longStrike);
  pthread_join(*pt,NULL);
  pthread_join(*pt2,NULL);
  pthread_join(*pt3,NULL);
  free(pt);
  free(pt2);
  free(pt3);


  for(int i = 0; i < 8; i++){
    keyts[i] = strikeTone(keys[i],longStrike);
  }

  for(int i = 0; i < 8; i++){
    pthread_join(*keyts[i],NULL); 
    free(keyts[i]);
  }
 
  m->sstop();

}
