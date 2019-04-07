CXX=g++ 
CPPFLAGS=-std=c++11 -I ./include/ -pthread

all : chat_client chat_server

chat_client: chat_client.o
	$(CXX) -o chat_client chat_client.o $(CPPFLAGS)
 
chat_server: chat_server.o
	$(CXX) -o chat_server chat_server.o $(CPPFLAGS)

chat_client.o: chat_client.cpp chat_message.hpp
	$(CXX) $(CPPFLAGS) -c chat_client.cpp

chat_server.o: chat_server.cpp chat_message.hpp
	$(CXX) $(CPPFLAGS) -c chat_server.cpp

clean:
	-rm *.o
