#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <ApplicationServices/ApplicationServices.h>

#include "Mahalo.h"
#include "SampleMixer.h"
#include "Wave.h"
#include "tones.h"

int main(int argc, char *argv[]){

  float maxX = atoi(argv[1]);
  float maxY = atoi(argv[2]);

  Mahalo *m = new Mahalo();
  m->setup();

  float tones[16] = { C4, Csharp4, D4, Dsharp4, 
                      E4, F5, Fsharp5, G5,
                      Gsharp5, A5, Asharp5, B5,
                      C6, Csharp6, D6, Dsharp6 };

  Wave *waves[16];

  SampleMixer *mix = new SampleMixer();
  for(int i = 0; i < 16; i++){
    waves[i] = new Wave(tones[i],m->getRate(),0.0);
    mix->Add(waves[i]);
  }
  
  m->setSampleSource(mix); 
  
  m->sstart();

  int fd;
  unsigned char n[3];
  int which = 0;
  unsigned delay = 0;
  float level = 0.0;
  unsigned freq;

  fd = open(argv[1],O_RDONLY);
  while( read(fd,n,3) ){

      CGEventRef event = CGEventCreate(NULL);
      CGPoint cursor = CGEventGetLocation(event);
      CFRelease(event);
      fprintf(stderr,"%f %f\n",cursor.x, cursor.y);
 
      delay = (int)((cursor.y / maxY) * 1000); 
      freq = (int)((cursor.x / maxX) * 16);

      waves[freq]->setAmpVal(1.0 - (cursor.y / maxY));
      usleep(delay);
      waves[freq]->setAmpVal(0.0);
      usleep(3000);

  }


  usleep(5000000);

  m->sstop();
}
