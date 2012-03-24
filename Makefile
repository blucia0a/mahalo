SRCS= SampleMixer.cpp Mahalo.cpp Wave.cpp BeatWave.cpp Noiser.cpp FilePlayer.cpp

all:
	g++ -m32 -march=i386 -Wno-deprecated-declarations -shared -fPIC $(SRCS) -o libmahalo.so -framework CoreAudio  

beatwavetest: BeatWaveTest.cpp BeatWave.cpp BeatWave.h
	g++  -m32 -march=i386 BeatWaveTest.cpp -o BeatWaveTest -L. -lmahalo -framework CoreAudio

keysynth: KeySynth.c
	gcc KeySynth.c -o KeySynth -framework CoreAudio -framework ApplicationServices -framework CoreMidi -framework AudioToolbox -framework AudioUnit
test:
	g++ CoreMidiTest.cpp -o CoreMidiTest -framework CoreAudio -framework ApplicationServices -framework CoreMidi -framework AudioToolbox -framework AudioUnit
	g++ WaveTest.cpp -o WaveTest -L. -lmahalo -framework CoreAudio
	g++ -Wno-deprecated-declarations KeybdTest.cpp -L. -lmahalo -o KeybdTest -framework CoreAudio -framework ApplicationServices
	g++ -Wno-deprecated-declarations MouseTest.cpp -L. -lmahalo -o MouseTest -framework CoreAudio -framework ApplicationServices
	g++ -Wno-deprecated-declarations HistoTest.cpp -L. -lmahalo -o HistoTest -framework CoreAudio
	g++ NoiserTest.cpp -o NoiserTest -L. -lmahalo -framework CoreAudio
	g++ FPTest.cpp -o FPTest -L. -lmahalo -framework CoreAudio
	g++ WaveTest.cpp -o WaveTest -L. -lmahalo -framework CoreAudio
	g++ PolyPhonyTest.cpp -o PolyPhonyTest -L. -lmahalo -framework CoreAudio
	g++ -g KeyBoardTest.cpp -o KeyBoardTest -lcurses -L. -lmahalo -framework CoreAudio

clean:
	rm *.o ./NoiserTest ./WaveTest libmahalo.so
