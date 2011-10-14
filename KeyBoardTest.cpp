#include "Mahalo.h"
#include "SampleMixer.h"
#include "Wave.h"
#include "tones.h"
#include <unistd.h>
#include <pthread.h>
#include <ncurses.h>
#include <sys/time.h>

Wave *keys[12];
unsigned long attempts[12];
bool strike[12];

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



void shortStrike(int i){

  Wave *w = keys[i];

/*I'm sorry...*/
start:
  for(int i = 0; i < 100; i++){
    usleep(500);
    if(strike[i] == true){ 
      if(attempts[i] > 1000){
        strike[i] = false; w->setAmpVal(0); goto start; 
        attempts[i] = 0;
      }else{
        attempts[i]++;
      }
    }
    w->setAmpVal(w->getAmpVal() + .01);
  }

  usleep(20000); 

  for(int i = 0; i < 100; i++){
    usleep(300);
    if(strike[i] == true){ 
      if(attempts[i] > 1000){
        strike[i] = false; w->setAmpVal(0); goto start; 
        attempts[i] = 0;
      }else{
        attempts[i]++;
      }
    }
    w->setAmpVal(w->getAmpVal() - .010);
  }
  w->setAmpVal(0);

}

void *runTone(void *w){
  unsigned long id = (unsigned long )w;
  while(1){

    if( strike[id] ){
      strike[id] = 0;
      attempts[id] = 0;
      shortStrike(id);
    }
  
  } 
}

int main(int argc, char *argv[]){


  WINDOW *main_win;
  initscr();
  clear();
  noecho();
  cbreak();

  main_win = newwin(30, 10, 0, 0);

  Mahalo *m = new Mahalo();
  m->setup();

  SampleMixer *mix1 = new SampleMixer();

  keys[0] = new Wave(C4,m->getRate(),0); 
  keys[1] = new Wave(Csharp4,m->getRate(),0); 
  keys[2] = new Wave(D4,m->getRate(),0); 
  keys[3] = new Wave(Dsharp4,m->getRate(),0); 
  keys[4] = new Wave(E4,m->getRate(),0); 
  keys[5] = new Wave(F4,m->getRate(),0); 
  keys[6] = new Wave(Fsharp4,m->getRate(),0); 
  keys[7] = new Wave(G4,m->getRate(),0); 
  keys[8] = new Wave(Gsharp4,m->getRate(),0); 
  keys[9] = new Wave(A4,m->getRate(),0); 
  keys[10] = new Wave(Asharp4,m->getRate(),0); 
  keys[11] = new Wave(B4,m->getRate(),0); 
 
  pthread_t keyts[12];

  for(int i = 0; i < 12; i++){
    attempts[i] = 0;
    strike[i] = false;
    mix1->Add(keys[i]);
    pthread_create(&(keyts[i]),NULL,runTone,(void*)i);
  }

  m->setSampleSource(mix1); 
  m->sstart();
 
  int c; 
  while(1){
    int key = -1;
    c = getch();
    switch(c){
      case 'a':
        key = 0; 
        break;
      case 'w':
        key = 1; 
        break;
      case 's':
        key = 2; 
        break;
      case 'e':
        key = 3; 
        break;
      case 'd':
        key = 4; 
        break;
      case 'r':
        key = 5; 
        break;
      case 'f':
        key = 6; 
        break;
      case 'g':
        key = 7; 
        break;
      case 'y':
        key = 8; 
        break;
      case 'h':
        key = 9; 
        break;
      case 'u':
        key = 10; 
        break;
      case 'j':
        key = 11; 
        break;
      default:
        key = 0;
        break;
    }
    strike[key] = true;

  }
  
  m->sstop();

}
