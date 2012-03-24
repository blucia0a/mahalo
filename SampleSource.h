#ifndef _SAMPLE_SOURCE_H_
#define _SAMPLE_SOURCE_H_
/*
 *  SampleSource.h
 *  Sin
 *
 *  Created by blucia0a on 8/25/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

class SampleSource{
public:
  virtual void getNextSample(float *out) = 0;
  virtual void setAmpVal(float newamp) = 0;
};
#endif
