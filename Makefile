CC=gcc
CFlags=-Wall -g -fsanitize=address

main2.exe: main.cpp Window.cpp Graphics.cpp Utility.cpp App.cpp Tools.cpp Functors.cpp Events.cpp
	$(CC) $(CFlags) main.cpp Window.cpp Graphics.cpp Utility.cpp App.cpp Tools.cpp Functors.cpp Events.cpp -o main2.exe
