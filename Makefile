CC=g++
CFlags=-g

main2.exe: main.cpp Window.cpp Graphics.cpp Utility.cpp App.cpp Tools.cpp Functors.cpp Events.cpp CustomTools.cpp PluginApi.cpp Slider.cpp
	$(CC) $(CFlags) main.cpp Window.cpp Graphics.cpp Utility.cpp App.cpp Tools.cpp Functors.cpp Events.cpp CustomTools.cpp PluginApi.cpp Slider.cpp -o main2.exe
