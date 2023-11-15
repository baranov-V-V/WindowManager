CC=g++
CFlags=-c -Wall

all: main2.exe

main.o: main.cpp App.cpp
	$(CC) $(CFlags) main.cpp App.cpp

Window.o: Window.cpp Functors.cpp App.cpp Tools.cpp
	$(CC) $(CFlags) Window.cpp Functors.cpp App.cpp Tools.cpp
	
Graphics.o: Graphics.cpp Window.cpp App.cpp
	$(CC) $(CFlags) Graphics.cpp Window.cpp App.cpp

Utility.o: Utility.cpp Functors.cpp Window.cpp App.cpp			
	$(CC) $(CFlags) Utility.cpp Functors.cpp Window.cpp App.cpp

App.o: App.cpp Tools.cpp Functors.cpp Window.cpp
	$(CC) $(CFlags) App.cpp Tools.cpp Functors.cpp Window.cpp

Tools.o: Tools.cpp Window.cpp
	$(CC) $(CFlags) Tools.pcp Window.cpp

Functors.o: Functors.cpp Window.cpp
	$(CC) $(CFlags) Functors.cpp Window.cpp

main2.exe:
	$(CC) main.o Window.o Graphics.o Utility.o App.o Tools.o Functors.o -o main2.exe
