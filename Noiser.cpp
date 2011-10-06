/*
 *  Wave.cpp
 *  Sin
 *
 *  Created by blucia0a on 8/25/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "Noiser.h"
#include <math.h>
#include <stdio.h>
Noiser::Noiser(float (*gen)(int)){
	
    // initialize phase and de-zipper filters.
    phase = 0;
    this->gen = gen;
  
}

void Noiser::getNextSample(float *out){

  int t = phase;
  out[0] = gen(t);
  out[1] = gen(t);

  phase++;

}

void Noiser::setAmpVal(double val){
    this->amp = val;
}

void Noiser::setFreqVal(double val){
    this->freq = 0;
}

double Noiser::getFreqVal(){
  return 0;
}
double Noiser::getAmpVal(){
    return this->amp;
}
