/*
 *  SampleMixer.cpp
 *  Sin
 *
 *  Created by blucia0a on 8/6/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "SampleMixer.h"
#include <stdlib.h>
#include <string.h>
SampleMixer::SampleMixer(){

  numSources = 0;
  srcs = (SampleSource **)malloc(sizeof(SampleSource *) * MIXER_MAX_SRCS);
  memset(srcs,0,sizeof(SampleSource *) * MIXER_MAX_SRCS);
   
}

void SampleMixer::Mix(int numSources, SampleSource **srcs, float *out){
	
	out[0] = 0.0; //left
	out[1] = 0.0; //right
	for(int i = 0; i < numSources; i++){
		float o[2];
	    srcs[i]->getNextSample(o);
		out[0] = out[0] + o[0];// > 1.0 ? 0.9 : out[0] + o[0];
		out[1] = out[1] + o[1];// > 1.0 ? 0.9 : out[1] + o[1];
	}
	out[0] /= (float)numSources;
	out[1] /= (float)numSources;
}

void SampleMixer::getNextSample(float *out){
  this->Mix(this->numSources,this->srcs,out);
}

bool SampleMixer::Add(SampleSource *s){
  for(int i = 0; i < MIXER_MAX_SRCS; i++){
    if(this->srcs[i] == NULL){
      this->srcs[this->numSources++] = s;
      return true;
    }
  }
  return false;
}


SampleSource *SampleMixer::Remove(unsigned i){
  SampleSource *r = NULL;
  if(this->srcs[i] == NULL){
    r = this->srcs[i];
    this->srcs[i] = NULL;
  }
  return r;
}
