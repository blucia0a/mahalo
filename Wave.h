#ifndef _WAVE_H_
#define _WAVE_H_
/*
 *  Wave.h
 *  Sin
 *
 *  Created by blucia0a on 8/25/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */
#include "SampleSource.h"
class Wave : public SampleSource{
	double freq, amp;		// controls
	double freqz, ampz;	    // for dezipper filter
	double phase;			// oscillator phase in radians
	double srate;
public:	
	Wave(double frq, double rate, double ampl);
	void setAmpVal(double val);
	void setFreqVal(double val);
	virtual void getNextSample(float *out);
	
};
#endif