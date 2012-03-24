#include <CoreAudio/CoreAudio.h>
#include <AudioToolbox/AudioToolbox.h>
#include <ApplicationServices/ApplicationServices.h>
#include <time.h>
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
    AudioUnit *SynthUnit = (AudioUnit *)refcon;
    
    if((type == kCGEventMouseMoved)){  
      CGPoint cursor = CGEventGetLocation(event);
      float my = cursor.y;
      float frac = my/900.; 
      int newval = (int)((float)65535 * frac);
      
      fprintf(stderr,"%d\n",newval);

      MusicDeviceMIDIEvent(*SynthUnit,  224, (newval & 0xFF), ( newval & 0xFF00 ) >> 8, 0);
      return event;

    }

    if ((type != kCGEventKeyDown) && (type != kCGEventKeyUp)){
        return event;
    }

    // The incoming keycode.
    CGKeyCode keycode = (CGKeyCode)CGEventGetIntegerValueField(
                                       event, kCGKeyboardEventKeycode);
    int tone;
    int which = -1;
    switch(keycode){

      case kKeyboardAKey:
        which = 0;
      break;
      
      case kKeyboardWKey:
        which = 1;
      break;

      case kKeyboardSKey:
        which = 2;
      break;
      
      case kKeyboardEKey:
        which = 3;
      break;

      case kKeyboardDKey:
        which = 4;
      break;
      
      case kKeyboardRKey:
        which = 5;
      break;
      
      case kKeyboardFKey:
        which = 6;
      break;
      
      case kKeyboardGKey:
        which = 7;
      break;

      case kKeyboardYKey:
        which = 8;
      break;

      case kKeyboardHKey:
        which = 9;
      break;

      case kKeyboardUKey:
        which = 10;
      break;

      case kKeyboardJKey:
        which = 11;
      break;

      case kKeyboardKKey:
        which = 12;
      break;


      case kKeyboardOKey:
        if( voice > 0 ){ 
          voice--;
        }
        fprintf(stderr,"Setting to voice %d\n",voice);
        MusicDeviceMIDIEvent(SynthUnit, 0xC0, voice, 0, 0);

      break;
      
      case kKeyboardPKey:
        if( voice < 127 ){ 
          voice++;
        }
        fprintf(stderr,"Setting to voice %d\n",voice);
        MusicDeviceMIDIEvent(SynthUnit, 0xC0, voice, 0, 0);
      break;
      default:
      break; 
    }

    if( which == -1 ){ return event; }
    if( type == kCGEventKeyDown && !Keys[which].down){
      Keys[which].down = true;
      MusicDeviceMIDIEvent(*SynthUnit, kMIDINoteOn, Keys[which].tone, 127, 0);
    }

    if( type == kCGEventKeyUp ){
      MusicDeviceMIDIEvent(*SynthUnit, kMIDINoteOff, Keys[which].tone, 127, 0);
      Keys[which].down = false;
    }

    return event; 

}


int main(int argc, char *argv[])
{


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


  /*Synth Node*/
  AUGraphOpen(AudioGraph);
  AUGraphInitialize(AudioGraph);
  AUGraphStart(AudioGraph);

  AUNode SynthNode;
  AudioUnit SynthUnit;

  cd.componentManufacturer = kAudioUnitManufacturer_Apple;
  cd.componentFlags = 0;
  cd.componentFlagsMask = 0;
  cd.componentType = kAudioUnitType_MusicDevice;
  cd.componentSubType = kAudioUnitSubType_DLSSynth;

  AUGraphNewNode(AudioGraph, &cd, 0, NULL, &SynthNode);
  AUGraphGetNodeInfo(AudioGraph, SynthNode, 0, 0, 0, &SynthUnit);

  AUGraphConnectNodeInput(AudioGraph, SynthNode, 0, MixerNode, 0);

  AUGraphUpdate(AudioGraph, NULL);

  /*Show the graph state*/ 
  CAShow(AudioGraph);
 
  int drd = rand() % 128; 
  MusicDeviceMIDIEvent(SynthUnit, 0xC0, drd, 0, 0);
  fprintf(stderr,"%d\n",drd);
  //MusicDeviceMIDIEvent(SynthUnit, 0xC0, 122, 0, 0);
  MusicDeviceMIDIEvent(SynthUnit, kMIDICtrl, 91, 0, 0);

  CGEventMask mask = CGEventMaskBit(kCGEventKeyDown) |
                     CGEventMaskBit(kCGEventKeyUp) | 
                     CGEventMaskBit(kCGEventMouseMoved);

  CFMachPortRef r = CGEventTapCreate( kCGSessionEventTap, 
                                      kCGHeadInsertEventTap, 
                                      kCGEventTapOptionDefault, 
                                      mask, keyPressed, &SynthUnit);

  CFRunLoopSourceRef loop = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, r, 0);
  CFRunLoopAddSource(CFRunLoopGetCurrent(),loop,kCFRunLoopCommonModes);
  CGEventTapEnable(r,true);

  CFRunLoopRun();

  return(0);
}
