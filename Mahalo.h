#ifndef _SIN_GEN_H_
#define _SIN_GEN_H_
/*
 *  Mahalo.h
 *  Sin
 *
 *  Created by blucia0a on 8/25/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

//#include <Cocoa/Cocoa.h>
#include <CoreAudio/AudioHardware.h>
#include "Wave.h"
#include "SampleMixer.h"
#include "tones.h"

class Mahalo {

public:	
	
  Boolean		initialized;	// successful init?
  Boolean		soundPlaying;	// playing now?
  AudioDeviceID	device;		// the default device
  UInt32	deviceBufferSize;	// bufferSize returned by kAudioDevicePropertyBufferSize
  AudioStreamBasicDescription	deviceFormat;	// info about the default device
  double pan, panz;
  SampleSource *src;
  
  Mahalo();
  void setup();
  bool sstart();
  bool sstop();
  void setSampleSource(SampleSource *s);
  float getRate();
	
	
};

#endif
