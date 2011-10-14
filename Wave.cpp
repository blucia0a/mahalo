/*
 *  Wave.cpp
 *  Sin
 *
 *  Created by blucia0a on 8/25/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "Wave.h"
#include <math.h>
#include <stdlib.h>
Wave::Wave(double frq, double srate, double amp = 0.5){
	
    //Sample rate is passed in from the HAL deviceInfo structure
    this->srate = srate;
	
    // initialize phase and de-zipper filters.
    phase = 0.0;
    this->freqz = this->freq = frq * 2. * 3.14159265459 / srate; 
    this->ampz = this->amp = amp;
    this->callback = NULL;

}

void Wave::setCallback(void (*f)(Wave *, void *), void *cbd){

  this->callback = f;
  this->cbdata = cbd;

}

double Wave::getRate(){
  return this->srate;
}

void Wave::getNextSample(float *out){
	
  float wave = sin(this->phase) * this->ampz;  // generate sine wave
  this->phase = this->phase + this->freqz;  // increment phase
  
  // write output
  out[0] = wave;	// left channel
  out[1] = wave;	// right channel
  
  // de-zipper controls
  this->ampz  = 0.001 * this->amp  + 0.999 * this->ampz;
  this->freqz = 0.001 * this->freq + 0.999 * this->freqz;	

  if(this->callback != NULL){
    this->callback(this, this->cbdata);
  }

}

void Wave::setAmpVal(double val){
    this->amp = val;
}

void Wave::setFreqVal(double val){
    this->freq = val * 2. * 3.14159265359 / this->srate;
}

double Wave::getAmpVal(){
    return this->amp;
}

double Wave::getFreqVal(){
    return this->freq / 2. / 3.141592653589 * this->srate;
}
