SRCS= SampleMixer.cpp Mahalo.cpp Wave.cpp Noiser.cpp FilePlayer.cpp

all: Periodic.c
	g++ Periodic.c -std=c99 -o Periodic -framework CoreAudio -framework ApplicationServices -framework CoreMidi -framework AudioToolbox -framework AudioUnit

clean:
	rm *.o ./NoiserTest ./WaveTest libmahalo.so
