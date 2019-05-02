CXX=g++

CXXFLAGS= -Wall -O0 -g -std=c++11

CPPFLAGS=-I ./chat/include/

all: chat_client chat_server
	mv chat_client -t .. && mv chat_server -t ..

chat_client.o: chat_client.cpp chat_message.hpp 

chat_server.o: chat_server.cpp chat_message.hpp

chat_client: chat_client.o
	${CXX} -o chat_client chat_client.o SuperChat_GUI.cpp -lpthread -lncurses ${CPPFLAGS}
chat_server: chat_server.o
	${CXX} -o chat_server chat_server.o -lpthread
clean:
	rm -f *.o && cd .. && rm -f chat_client chat_server ChatRooms && rm -rf rooms && echo "Cleaned up"
