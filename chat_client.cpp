//
// chat_client.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <stdlib.h>

#include <deque>
#include <iostream>
#include <thread>
#include <string.h>
#include <stdio.h>

#include "asio.hpp"
#include "chat_client_cheader.hpp"
#include "chat_message.hpp"
#include "SuperChat.h"
#include <ncurses.h>


using asio::ip::tcp;


int main(int argc, char* argv[])
{
  try
  {
    /*if (argc != 3)
    {
      std::cerr << "Usage: chat_client <host> <port>\n";
      return 1;
    }*/

    asio::io_context io_context;

    tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve("127.0.0.1", "9000"); //originally argv[1] and argv[2]
    chat_client c(io_context, endpoints);
    chat_client* client = &c; //dlete
    std::thread t([&io_context](){ io_context.run(); });
    //chat_message msg;//move back into while loop if doesn't work.
    
    Client_Window A;
    A.GUI_main(client); //Begins the GUI. Modified to take in a client pointer.

    /*char input_mssg[chat_message::max_body_length + 1];
    while (std::cin.getline(input_mssg, chat_message::max_body_length + 1))
    {
      
      //std::strcat(line, " -Jezzy testing. "); //Edited 1s
      msg.body_length(std::strlen(input_mssg));
      std::memcpy(msg.body(), line, msg.body_length());
      msg.encode_header();
      c.write(msg);
    }*/

    c.close();
    t.join();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
