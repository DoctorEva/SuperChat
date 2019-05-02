//
// chat_client.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Modified by  Jezreel Aquitania, Douglas Zenger, Thomas Tran
// Copyright 2019

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
    if (argc != 3)
    {
      std::cerr << "Usage: chat_client <host> <port>\n";
      return 1;
    }

    asio::io_context io_context;

    tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve(argv[1], argv[2]);
    chat_client c(io_context, endpoints);
    chat_client* client = &c;
    std::thread t([&io_context](){ io_context.run(); });


    Client_Window A;
    A.GUI_main(client); //Begins the GUI. Modified to take in a client pointer.

    c.close();
    t.join();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
