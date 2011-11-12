SRCS= SampleMixer.cpp Mahalo.cpp Wave.cpp Noiser.cpp FilePlayer.cpp

all:
	g++ -Wno-deprecated-declarations -shared -fPIC $(SRCS) -o libmahalo.so -framework CoreAudio  

test:
	g++ -Wno-deprecated-declarations MouseTest.cpp -L. -lmahalo -o MouseTest -framework CoreAudio -framework ApplicationServices
	g++ -Wno-deprecated-declarations HistoTest.cpp -L. -lmahalo -o HistoTest -framework CoreAudio
	g++ NoiserTest.cpp -o NoiserTest -L. -lmahalo -framework CoreAudio
	g++ FPTest.cpp -o FPTest -L. -lmahalo -framework CoreAudio
	g++ WaveTest.cpp -o WaveTest -L. -lmahalo -framework CoreAudio
	g++ PolyPhonyTest.cpp -o PolyPhonyTest -L. -lmahalo -framework CoreAudio
	g++ -g KeyBoardTest.cpp -o KeyBoardTest -lcurses -L. -lmahalo -framework CoreAudio

clean:
	rm *.o ./NoiserTest ./WaveTest libmahalo.so
