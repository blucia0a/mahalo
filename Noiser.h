#ifndef _NOISER_H_
#define _NOISER_H_
/*
 *  Wave.h
 *  Sin
 *
 *  Created by blucia0a on 8/25/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */
#include "SampleSource.h"
class Noiser : public SampleSource{
	double freq, amp;		// controls
	double freqz, ampz;	    // for dezipper filter
	int phase;			// oscillator phase in radians
	double srate;
        bool isActive;
public:	
	Noiser(float (*gen)(int));
        float (*gen)(int);
	void setAmpVal(double val);
	void setFreqVal(double val);
	double getFreqVal();
	double getAmpVal();
        bool active();
        void activate();
        void deactivate();
	virtual void getNextSample(float *out);
	
};
#endif
