CC=g++
CFlags=-Wall -g

main2.exe:
	$(CC) $(CFlags) main.cpp Window.cpp Graphics.cpp Utility.cpp App.cpp Tools.cpp Functors.cpp -o main2.exe
