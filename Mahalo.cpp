/*
 *  Mahalo.cpp
 *  Sin
 *
 *  Created by blucia0a on 8/25/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */
#include "Mahalo.h"

OSStatus sappIOProc (AudioDeviceID  inDevice, const AudioTimeStamp*  inNow, const AudioBufferList*   inInputData, 
           const AudioTimeStamp*  inInputTime, AudioBufferList*  outOutputData, const AudioTimeStamp* inOutputTime, 
           void* defptr)
{    
    Mahalo *def = (Mahalo*)defptr; 
    int i;
    
    int numSamples = def->deviceBufferSize / def->deviceFormat.mBytesPerFrame;
    
    float *out = (float*)outOutputData->mBuffers[0].mData;
    
    for (i=0; i<numSamples; ++i) {
        
      if( def->src != NULL ){
        def->src->getNextSample(out);
      }else{
        out[0] = 0;
        out[1] = 0;
      }
      
      out[0] = out[0] * (1.0 - def->panz);
      out[1] = out[1] * def->panz;
      
      //move the output pointer ahead in the hardware buffer
      out++; out++;
      
      //update the zippered pan value
      def->panz  = 0.001 * def->pan  + 0.999 * def->panz;
        
    }
    
    return kAudioHardwareNoError;     
}

/*Mahalo Implementation*/
Mahalo::Mahalo(){

}

float Mahalo::getRate(){
  return this->deviceFormat.mSampleRate;
}

void Mahalo::setSampleSource(SampleSource *s){
  src = s;
}

void Mahalo::setup(void){
  
    OSStatus      err = kAudioHardwareNoError;
    UInt32        count;    
    device = kAudioDeviceUnknown;
  
    initialized = false;
  
    // get the default output device for the HAL
    count = sizeof(device);    // it is required to pass the size of the data to be returned
    err = AudioHardwareGetProperty(kAudioHardwarePropertyDefaultOutputDevice,  &count, (void *) &device);
    if (err != kAudioHardwareNoError) {
      fprintf(stderr, "get kAudioHardwarePropertyDefaultOutputDevice error %d\n", err);
        return;
    }
    
    // get the buffersize that the default device uses for IO
    count = sizeof(deviceBufferSize);  // it is required to pass the size of the data to be returned
    err = AudioDeviceGetProperty(device, 0, false, kAudioDevicePropertyBufferSize, &count, &deviceBufferSize);
    if (err != kAudioHardwareNoError) {
      fprintf(stderr, "get kAudioDevicePropertyBufferSize error %d\n", err);
        return;
    }
    fprintf(stderr, "deviceBufferSize = %d\n", deviceBufferSize);
  
    // get a description of the data format used by the default device
    count = sizeof(deviceFormat);  // it is required to pass the size of the data to be returned
    err = AudioDeviceGetProperty(device, 0, false, kAudioDevicePropertyStreamFormat, &count, &deviceFormat);
    if (err != kAudioHardwareNoError) {
      fprintf(stderr, "get kAudioDevicePropertyStreamFormat error %d\n", err);
        return;
    }
    if (deviceFormat.mFormatID != kAudioFormatLinearPCM) {
      fprintf(stderr, "mFormatID !=  kAudioFormatLinearPCM\n");
        return;
    }
    if (!(deviceFormat.mFormatFlags & kLinearPCMFormatFlagIsFloat)) {
      fprintf(stderr, "Sorry, currently only works with float format....\n");
        return;
    }
    
    initialized = true;

    this->panz = this->pan = 0.5;
  
    fprintf(stderr, "mSampleRate = %g\n", deviceFormat.mSampleRate);
    fprintf(stderr, "mFormatFlags = %08X\n", deviceFormat.mFormatFlags);
    fprintf(stderr, "mBytesPerPacket = %d\n", deviceFormat.mBytesPerPacket);
    fprintf(stderr, "mFramesPerPacket = %d\n", deviceFormat.mFramesPerPacket);
    fprintf(stderr, "mChannelsPerFrame = %d\n", deviceFormat.mChannelsPerFrame);
    fprintf(stderr, "mBytesPerFrame = %d\n", deviceFormat.mBytesPerFrame);
    fprintf(stderr, "mBitsPerChannel = %d\n", deviceFormat.mBitsPerChannel);

}


bool Mahalo::sstart(){
  
    OSStatus    err = kAudioHardwareNoError;
  
    if (!initialized) return false;
    if (soundPlaying) return false;
    
    err = AudioDeviceAddIOProc(device, sappIOProc, (void *) this);  // setup our device with an IO proc
    if (err != kAudioHardwareNoError) return false;
    
    err = AudioDeviceStart(device, sappIOProc);        // start playing sound through the device
    if (err != kAudioHardwareNoError) return false;
  
    soundPlaying = true;            // set the playing status global to true
    return true;

}

bool Mahalo::sstop(){
  
    OSStatus   err = kAudioHardwareNoError;
    
    if (!initialized) return false;
    if (!soundPlaying) return false;
    
    err = AudioDeviceStop(device, sappIOProc);        // stop playing sound through the device
    if (err != kAudioHardwareNoError) return false;
  
    err = AudioDeviceRemoveIOProc(device, sappIOProc);      // remove the IO proc from the device
    if (err != kAudioHardwareNoError) return false;
    
    soundPlaying = false;            // set the playing status global to false
    return true;
  
}
