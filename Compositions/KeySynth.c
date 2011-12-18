#include <CoreAudio/CoreAudio.h>
#include <AudioToolbox/AudioToolbox.h>
#include <ApplicationServices/ApplicationServices.h>
#include <time.h>

int NUM_SYNTHS;
int doMouse;
AUNode *SynthNodes;
AudioUnit *SynthUnits;


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
    AudioUnit *SUs= (AudioUnit *)refcon;
    
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
    fprintf(stderr,"Synth %d: Voice %d\n",pps->id,pps->tone);
    MusicDeviceMIDIEvent(SynthUnits[pps->id], kMIDINoteOn, pps->tone, 127, 0);
    usleep( pps->duration ); 
    MusicDeviceMIDIEvent(SynthUnits[pps->id], kMIDINoteOff, pps->tone, 127, 0);
    usleep( pps->period); 
  }
   
}

int main(int argc, char *argv[])
{

  if( argc > 1 ){
    NUM_SYNTHS = atoi(argv[1]);
  }else{
    NUM_SYNTHS = 8;
  }
  SynthNodes = malloc(sizeof(AUNode) * NUM_SYNTHS);
  SynthUnits = malloc(sizeof(AudioUnit) * NUM_SYNTHS);

  if( argc > 2 ){
    doMouse = 1;
  }else{
    doMouse = 0;
  }

  srand(time(NULL));
  int i;
  for(i = 0; i < 13; i++){
    Keys[i].down = false;
    Keys[i].tone = 0x41 + (i);
  }
  

  NewAUGraph(&AudioGraph);

  /*Output Node*/
  AudioComponentDescription cd;
  AUNode OutputNode;
  AudioUnit OutputUnit;

  cd.componentManufacturer = kAudioUnitManufacturer_Apple;
  cd.componentFlags = 0;
  cd.componentFlagsMask = 0;
  cd.componentType = kAudioUnitType_Output;
  cd.componentSubType = kAudioUnitSubType_DefaultOutput;

  AUGraphNewNode(AudioGraph, &cd, 0, NULL, &OutputNode);
  AUGraphGetNodeInfo(AudioGraph, OutputNode, 0, 0, 0, &OutputUnit);

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

  AUGraphConnectNodeInput(AudioGraph, MixerNode, 0, OutputNode, 0);

  /*Distortion Node*/
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
  //AUGraphConnectNodeInput(AudioGraph, FXNode, 0, MixerNode, 0);



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
  
  /*Show the graph state*/ 

  pthread_t sources[NUM_SYNTHS];
  int instruments[NUM_SYNTHS]; 
  for(int i = 0; i < NUM_SYNTHS; i++){

    playParams *p = (playParams*)malloc(sizeof(playParams));
    p->id = i;
    p->tone = 0x2f + (rand() % 12);
    p->period =  (10000 + (rand() % 990000));
    p->duration =  (10000 + (rand() % 990000));

    //instruments[i] = 100 + (rand() % 28); 
    instruments[i] = 85 + (rand() % 43); 
    fprintf(stderr,"Synth %d: Voice %d\n",i,instruments[i]);
    MusicDeviceMIDIEvent(SynthUnits[i], 0xC0, instruments[i], 0, 0);
    pthread_create( &sources[i], NULL, periodic_play, (void *)p );
    
  }

  //MusicDeviceMIDIEvent(SynthUnit, kMIDICtrl, 91, 0, 0);

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
