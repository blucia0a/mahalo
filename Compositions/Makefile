SRCS= SampleMixer.cpp Mahalo.cpp Wave.cpp Noiser.cpp FilePlayer.cpp

all: Streamy.c
	#g++ Testy.c -std=c99 -g -o Testy -framework CoreAudio -framework ApplicationServices -framework CoreMidi -framework AudioToolbox -framework AudioUnit
	g++ Streamy.c -std=c99 -g -o Streamy -framework CoreAudio -framework ApplicationServices -framework CoreMidi -framework AudioToolbox -framework AudioUnit
	#g++ Periodic.c -std=c99 -g -o Periodic -framework CoreAudio -framework ApplicationServices -framework CoreMidi -framework AudioToolbox -framework AudioUnit
	#g++ HighPerio.c -std=c99 -g -o HighPerio -framework CoreAudio -framework ApplicationServices -framework CoreMidi -framework AudioToolbox -framework AudioUnit
	#g++ Chordiotic.c -std=c99 -g -o Chordiotic -framework CoreAudio -framework ApplicationServices -framework CoreMidi -framework AudioToolbox -framework AudioUnit
	#g++ Drumiotic.c -std=c99 -g -o Drumiotic -framework CoreAudio -framework ApplicationServices -framework CoreMidi -framework AudioToolbox -framework AudioUnit

clean:
	rm *.o ./NoiserTest ./WaveTest libmahalo.so
