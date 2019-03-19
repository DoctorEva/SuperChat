CXXFLAGS += -std=c++11
SUPERCHATFLAGS += -lncurses
ex: superchat.o
	$(CXX) $(CXXFLAGS) -o SuperChat SuperChat.o $(SUPERCHATFLAGS)
superchat.o: src/SuperChat.cpp src/SuperChat.h
	$(CXX) $(CXXFLAGS) -c src/SuperChat.cpp $(SUPERCHATFLAGS)
