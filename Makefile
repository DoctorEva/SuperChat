CXXFLAGS += -std=c++11
SUPERCHATFLAGS += -lncurses
ex: superchat_GUI.o src/main.cpp
	$(CXX) $(CXXFLAGS) -o SuperChat SuperChat_GUI.o src/main.cpp $(SUPERCHATFLAGS)
superchat_GUI.o: src/SuperChat_GUI.cpp src/SuperChat.h
	$(CXX) $(CXXFLAGS) -c src/SuperChat_GUI.cpp $(SUPERCHATFLAGS)
