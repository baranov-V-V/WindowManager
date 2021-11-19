CC=g++
CFlags=-Wall -g

main2.exe: main.cpp Window.cpp Graphics.cpp Utility.cpp App.cpp Tools.cpp Functors.cpp Events.cpp ModuleStandartFunc.cpp
	$(CC) $(CFlags) main.cpp Window.cpp Graphics.cpp Utility.cpp App.cpp Tools.cpp Functors.cpp Events.cpp ModuleStandartFunc.cpp -o main2.exe
