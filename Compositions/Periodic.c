#include <CoreAudio/CoreAudio.h>
#include <CoreFoundation/CFURL.h>
#include <CoreFoundation/CFBundle.h>
#include <AudioToolbox/AudioToolbox.h>
#include <AudioToolbox/ExtendedAudioFile.h>
#include <ApplicationServices/ApplicationServices.h>
#include "CAAudioUnitOutputCapturer.h"
#include <pthread.h>
#include <time.h>

int NUM_SYNTHS;
int g_level;
int cur_level;
int doMouse;
AUNode *SynthNodes;
AudioUnit *SynthUnits;
AudioUnit OutputUnit;
CAAudioUnitOutputCapturer *captor;

AUGraph AudioGraph;
enum {
  kMIDINoteOff = 128,
  kMIDINoteOn = 144,
  kMIDICtrl = 176,
  kMIDIPrgm = 192
};

int voice = 0;
enum {
    kKeyboardAKey     = 0x00,
    kKeyboardBKey     = 0x0b,
    kKeyboardCKey     = 0x08,
    kKeyboardDKey     = 0x02,
    kKeyboardEKey     = 0x0e,
    kKeyboardFKey     = 0x03,
    kKeyboardGKey     = 0x05,
    kKeyboardHKey     = 0x04,
    kKeyboardIKey     = 0x22,
    kKeyboardJKey     = 0x26,
    kKeyboardKKey     = 0x28,
    kKeyboardLKey     = 0x25,
    kKeyboardMKey     = 0x2e,
    kKeyboardNKey     = 0x2d,
    kKeyboardOKey     = 0x1f,
    kKeyboardPKey     = 0x23,
    kKeyboardQKey     = 0x0c,
    kKeyboardRKey     = 0x0f,
    kKeyboardSKey     = 0x01,
    kKeyboardTKey     = 0x11,
    kKeyboardUKey     = 0x20,
    kKeyboardVKey     = 0x09,
    kKeyboardWKey     = 0x0d,
    kKeyboardXKey     = 0x07,
    kKeyboardYKey     = 0x10,
    kKeyboardZKey     = 0x06,

    kKeyboardEscapeKey = 0x35,
    kKeyboardSpaceKey  = 0x31,
    kKeyboardCommaKey  = 0x2b,
    kKeyboardStopKey   = 0x2f,
    kKeyboardSlashKey  = 0x2c,
    kKeyboardColonKey  = 0x29,
    kKeyboardQuoteKey  = 0x27,
    kKeyboardLBrktKey  = 0x21
};


typedef struct _key{
  bool down;
  int tone;/*0x3c is middle c*/
} key;

key Keys[13];
CGEventRef keyPressed(
   CGEventTapProxy proxy,
   CGEventType type,
   CGEventRef event,
   void *refcon
){
    AudioUnit *SUs = (AudioUnit *)refcon;
    
    if((type == kCGEventMouseMoved)){  
      CGPoint cursor = CGEventGetLocation(event);
      float my = cursor.y;
      float frac = my/900.; 
      int newval = (int)((float)65535 * frac);
      
      //fprintf(stderr,"%d\n",newval);

      if( doMouse ){
        for(int i = 0; i < NUM_SYNTHS; i++){
          MusicDeviceMIDIEvent(SUs[i],  224, (newval & 0xFF), ( newval & 0xFF00 ) >> 8, 0);
        }
      }
      return event;

    }

    if ((type != kCGEventKeyDown) && (type != kCGEventKeyUp)){
        return event;
    }

    // The incoming keycode.
    CGKeyCode keycode = (CGKeyCode)CGEventGetIntegerValueField(
                                       event, kCGKeyboardEventKeycode);

    if( keycode == kKeyboardQKey && type == kCGEventKeyDown ){

      captor->Stop();
      captor->Close();
      exit(0);

    }


    return event; 

}

typedef struct _playParams{

  int id;
  unsigned long period;
  unsigned long duration;
  unsigned tone;
  
} playParams;

void * periodic_play(void *p){

  playParams *pps = (playParams*)p;
  while(1){
    MusicDeviceMIDIEvent(SynthUnits[pps->id], kMIDINoteOn, pps->tone, 127, 0);
    usleep( pps->duration ); 
    MusicDeviceMIDIEvent(SynthUnits[pps->id], kMIDINoteOff, pps->tone, 127, 0);
    usleep( pps->period); 
  }
   
}


typedef struct _sequenceParams{

  int id;

  int numSegments;

  unsigned long *period;
  unsigned long *duration;
  unsigned *tone;
  unsigned *target;
  
} sequenceParams;

sequenceParams *newSeq(int num){

  sequenceParams *n = (sequenceParams *)malloc( sizeof(sequenceParams) ); 
  n->period = (unsigned long *)malloc( num  * sizeof(unsigned long) );
  n->duration = (unsigned long *)malloc( num  * sizeof(unsigned long) );
  n->tone = (unsigned *)malloc( num  * sizeof(unsigned) );
  n->target = (unsigned *)malloc( num  * sizeof(unsigned) );
  n->numSegments = num; 
  return n;

}

void * sequence_play(void *p){

  sequenceParams *pps = (sequenceParams *)p;
  while(1){
  
    int error = 0;
    for(int i = 0; i < pps->numSegments; i++){

      if( i == 0 ){
        fprintf(stderr,"Round\n");
      }

   
      if( pps->id >= NUM_SYNTHS/2 ){ 

      MusicDeviceMIDIEvent(SynthUnits[pps->id], kMIDINoteOn, pps->tone[i], g_level, 0);
      usleep( pps->duration[i] ); 

      MusicDeviceMIDIEvent(SynthUnits[pps->id], kMIDINoteOff, pps->tone[i], g_level, 0);
      usleep( pps->period[i] ); 

      continue;
      } 

      MusicDeviceMIDIEvent(SynthUnits[pps->id], kMIDINoteOn, pps->tone[i], cur_level, 0);
      usleep( pps->duration[i] ); 

      MusicDeviceMIDIEvent(SynthUnits[pps->id], kMIDINoteOff, pps->tone[i], cur_level, 0);
      usleep( pps->period[i] ); 


      if( pps->tone[i] > pps->target[i] ){




        error += (pps->tone[i] - pps->target[i]); 
        if( rand() % 10 < 9 ){
          /*go lower - what should happen*/
          pps->tone[i] -= rand() % 3;
         
        }else{
          /*go higher - what shouldn't happen*/
          pps->tone[i] += 1;
        }
      }else if(pps->tone[i] < pps->target[i]){
        error += (pps->target[i] - pps->tone[i]); 
        if( rand() % 10 < 9 ){

          /*go higher- what should happen*/
          pps->tone[i] += rand() % 3;

        }else{

          /*go lower - what shouldn't happen*/
          pps->tone[i] -= 1;

        }

      }else{
/*
        if( rand() % 10 > 9 ){
          pps->tone[i] += 1;
        }
*/

      }

    }

    if( pps->id < NUM_SYNTHS/2 ){   
      fprintf(stderr,"Error: %d\n",error);
      if( cur_level < g_level ){ cur_level += 10;  if(cur_level > g_level){ cur_level = g_level; }}
    }

  }
   
}

int main(int argc, char *argv[])
{

  if( argc > 1 ){
    NUM_SYNTHS = atoi(argv[1]);
  }else{
    NUM_SYNTHS = 8;
  }
  SynthNodes = (AUNode *)malloc(sizeof(AUNode) * NUM_SYNTHS);
  SynthUnits = (AudioUnit*)malloc(sizeof(AudioUnit) * NUM_SYNTHS);

  if( argc > 2 ){
    g_level = atoi(argv[2]);
  }else{
    g_level = 64;
  }

  cur_level = g_level / 4;

  srand(time(NULL));
  int i;
  for(i = 0; i < 13; i++){
    Keys[i].down = false;
    Keys[i].tone = 0x41 + (i);
  }
  

  NewAUGraph(&AudioGraph);

  /*Output Node*/
  ComponentDescription cd;
  AUNode OutputNode;

  cd.componentManufacturer = kAudioUnitManufacturer_Apple;
  cd.componentFlags = 0;
  cd.componentFlagsMask = 0;
  cd.componentType = kAudioUnitType_Output;
  cd.componentSubType = kAudioUnitSubType_DefaultOutput;

  AUGraphNewNode(AudioGraph, &cd, 0, NULL, &OutputNode);
  AUGraphGetNodeInfo(AudioGraph, OutputNode, 0, 0, 0, &OutputUnit);


  /*Distortion Node*/
//aufc defr appl
  AUNode FXNode;
  AudioUnit FXUnit;
  cd.componentManufacturer = 'appl';
  cd.componentFlags = 0;
  cd.componentFlagsMask = 0;
  cd.componentType = 'aufx';
  cd.componentSubType = 'dist';
  AUGraphNewNode(AudioGraph, &cd, 0, NULL, &FXNode);
  AudioUnitSetParameter(FXUnit, kDistortionParam_Delay, kAudioUnitScope_Global, 0, 512, 0);
  AudioUnitSetParameter(FXUnit, kDistortionParam_Decay, kAudioUnitScope_Global, 0, 128, 0);
  AudioUnitSetParameter(FXUnit, kDistortionParam_DelayMix, kAudioUnitScope_Global, 0, 128, 0);
  AudioUnitSetParameter(FXUnit, kDistortionParam_SoftClipGain, kAudioUnitScope_Global, 0, 128, 0);
  AUGraphGetNodeInfo(AudioGraph, FXNode, 0, 0, 0, &FXUnit);
  AUGraphConnectNodeInput(AudioGraph, FXNode, 0, OutputNode, 0);

  /*Mixer Node*/
  AUNode MixerNode;
  AudioUnit MixerUnit;
  cd.componentManufacturer = kAudioUnitManufacturer_Apple;
  cd.componentFlags = 0;
  cd.componentFlagsMask = 0;
  cd.componentType = kAudioUnitType_Mixer;
  cd.componentSubType = kAudioUnitSubType_StereoMixer;

  AUGraphNewNode(AudioGraph, &cd, 0, NULL, &MixerNode);
  AUGraphGetNodeInfo(AudioGraph, MixerNode, 0, 0, 0, &MixerUnit);

  AUGraphConnectNodeInput(AudioGraph, MixerNode, 0, FXNode, 0);


  for(int i = 0; i < NUM_SYNTHS; i++ ){

    /*Synth Nodes*/
  
    AUGraphOpen(AudioGraph);
    AUGraphInitialize(AudioGraph);
    AUGraphStart(AudioGraph);
    //AUNode SynthNode;
    //AudioUnit SynthUnit;
  
    cd.componentManufacturer = kAudioUnitManufacturer_Apple;
    cd.componentFlags = 0;
    cd.componentFlagsMask = 0;
    cd.componentType = kAudioUnitType_MusicDevice;
    cd.componentSubType = kAudioUnitSubType_DLSSynth;
  
    AUGraphNewNode(AudioGraph, &cd, 0, NULL, &SynthNodes[i]);
    AUGraphGetNodeInfo(AudioGraph, SynthNodes[i], 0, 0, 0, &SynthUnits[i]);
  
    AUGraphConnectNodeInput(AudioGraph, SynthNodes[i], 0, MixerNode, i);
  
    AUGraphUpdate(AudioGraph, NULL);

    CAShow(AudioGraph);

  }


  pthread_t sources[NUM_SYNTHS];
  int instruments[NUM_SYNTHS]; 
  for(int i = 0; i < NUM_SYNTHS; i++){

    //playParams *p = (playParams*)malloc(sizeof(playParams));
    int numsegs;
    if( i < (NUM_SYNTHS/2) ){ 
      numsegs = 12;// + (rand() % 128); 
    }else{
      numsegs = 12 + (rand() % 32); 
    }

    sequenceParams *s = newSeq( numsegs ); 
    s->id = i;
  
    for( int j = 0; j < numsegs; j++ ){

      //s->period[i] =  (100000 + (rand() % 2000000));

      if( i < (NUM_SYNTHS/2) ){

        s->target[j] = 0x20 + j;
        s->tone[j] = 0x25 + (rand() % 36);

        if( j == 0 ){ 

          s->period[j] =  400000;//(10000 + (rand() % 1000000));
          s->duration[j] =  400000;//(10000 + (rand() % 1000000));

        }else{


          if( rand() % 2 ){
            if( rand() % 2 ){
              s->period[j] = s->period[j-1] * 2;
            }else{
              s->period[j] = s->period[j-1] / 2;
            }
          }else{
            s->period[j] = s->period[j-1];
          }

          if( rand() % 2 ){
            if( rand() % 2 && s->duration[j] <= 1000000){
              s->duration[j] = s->duration[j-1] * 2;
            }else{
              s->duration[j] = s->duration[j-1] / 2;
            }
          }else{
            s->duration[j] = s->duration[j-1];
          }
            
        }
        s->period[j] =  200000;//(10000 + (rand() % 1000000));
        s->duration[j] =  200000;//(10000 + (rand() % 1000000));

      }else{

        s->tone[j] = 0x25 + (rand() % 16);
        s->period[j] =  (10000 + (rand() % 20000));
        s->duration[j] =  (10000 + (rand() % 20000));

      }
    

    }
      
    if( i < (NUM_SYNTHS/2) ){

      instruments[i] = 12;

    }else{

      instruments[i] = 116;

    }

    //instruments[i] = 100 + (rand() % 28); 
    //instruments[i] = 112;//85 + (rand() % 43); 

    //instruments[i] = rand() % 2 == 0 ? 112 : 117;
    //if( rand() % 2 ){ instruments[i] = 112; }
    //fprintf(stderr,"Synth %d: Voice %d\n",i,instruments[i]);
    //MusicDeviceMIDIEvent(SynthUnits[i], kMIDICtrl, 91, 0, 0);
    MusicDeviceMIDIEvent(SynthUnits[i], 0xC0, instruments[i], 0, 0);
    pthread_create( &sources[i], NULL, sequence_play, (void *)s );
    
  }



  //CFURL fileurl = CFURLCreateWithFileSystemPath(NULL, CFSTR("recording.caf"), kCFURLPOSIXPathStyle, false);
  AudioDeviceID device;		// the default device
  AudioStreamBasicDescription anASBD;	// info about the default device

  anASBD.mFormatID = kAudioFormatLinearPCM;
  anASBD.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsBigEndian | kAudioFormatFlagIsPacked;
  anASBD.mSampleRate = 44100;
  anASBD.mChannelsPerFrame = 2;
  anASBD.mFramesPerPacket = 1;
  anASBD.mBytesPerPacket = anASBD.mChannelsPerFrame * sizeof (SInt16);
  anASBD.mBytesPerFrame = anASBD.mChannelsPerFrame * sizeof (SInt16);
  anASBD.mBitsPerChannel = 16;
 
  CFURLRef fileurl = CFURLCreateWithFileSystemPath(NULL, CFSTR("recording.caf"), kCFURLPOSIXPathStyle, false);
  captor = new CAAudioUnitOutputCapturer(OutputUnit, fileurl, 'caff', anASBD);
  fprintf(stderr,"%p\n",OutputUnit);
  captor->Start();
  int err = AudioUnitAddRenderNotify(MixerUnit, CAAudioUnitOutputCapturer::RenderCallback, captor);
  fprintf(stderr,"%lx\n",err);

  CGEventMask mask = CGEventMaskBit(kCGEventKeyDown) |
                     CGEventMaskBit(kCGEventKeyUp) | 
                     CGEventMaskBit(kCGEventMouseMoved);

  CFMachPortRef r = CGEventTapCreate( kCGSessionEventTap, 
                                      kCGHeadInsertEventTap, 
                                      kCGEventTapOptionDefault, 
                                      mask, keyPressed, SynthUnits);

  CFRunLoopSourceRef loop = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, r, 0);
  CFRunLoopAddSource(CFRunLoopGetCurrent(),loop,kCFRunLoopCommonModes);
  CGEventTapEnable(r,true);

  CFRunLoopRun();

  return(0);
}
