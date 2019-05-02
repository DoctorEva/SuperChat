### SuperChat Overview ###

This program was written for CSE3310 Introduction to Software Engineering. The program is a simple ncurses based chat client-server program developed to gain experience in developing an application using software engineering standards and working with a team to accomplish a goal.

----------

### Compilation ###
To compile and run the program. Assuming a Linux Terminal is being used:

From the SuperChat/ repository. Invoke the following commands:

1. cd src/
2. make clean; make;
3. cd ..

----------

### Running the server ###

From the SuperChat/ repository after compiling. Invoke the following commands:

1. sudo ./chat_server <port> [<port> ...]

Example: sudo ./chat_server 9000

### Running the client ###

From the SuperChat/ repository after compiling. Invoke the following command:

1. ./chat_client <host> <port>

Example: ./chat_client 127.0.0.1 9000


