//
// chat_server.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <sys/stat.h>
#include <sys/types.h>
#include<stdlib.h>
#include<stdio.h>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <utility>
#include "asio.hpp"
#include "chat_message.hpp"
#include "chat_server_cheader.hpp"

using asio::ip::tcp;

int main(int argc, char* argv[])
{
  if(!remove("ChatRooms"))
    printf("Removed old ChatRooms file\n");
  if(!remove("rooms")) // TODO - This currently doesnt work because "rooms" is not empty. Maybe this doesnt need to be fixed.
    printf("Removed old rooms directory\n");

  FILE* spawn = fopen("ChatRooms", "w");
  if(spawn)
  {
    fputs("Lobby\n",spawn);
    fclose(spawn);
    spawn = NULL;
    printf("Created new ChatRoom file\n");
    if(!chmod("ChatRooms", S_IRWXU | S_IRWXG | S_IRWXO))
      printf("Set ChatRooms permissions\n");
  }
  // Set appropriate access rules for newly created files.
  mkdir("rooms", S_IRWXU | S_IRWXG | S_IRWXO);
  if(!chmod("rooms", S_IRWXU | S_IRWXG | S_IRWXO))
    printf("Set rooms directory permissions\n");


  spawn = fopen("./rooms/Lobby", "w");
  if(spawn)
  {
    fputs("SERVER: Server Started\n", spawn);
    fclose(spawn);
    printf("Created new Lobby file\n");
    if(!chmod("./rooms/Lobby", S_IRWXU | S_IRWXG | S_IRWXO))
      printf("Set Lobby permissions\n");
  }


  try
  {
    if (argc < 2)
    {
      std::cerr << "Usage: chat_server <port> [<port> ...]\n";
      return 1;
    }

    asio::io_context io_context;

    std::list<chat_server> servers;
    for (int i = 1; i < argc; ++i)
    {
      tcp::endpoint endpoint(tcp::v4(), std::atoi(argv[i]));
      servers.emplace_back(io_context, endpoint);
    }

    io_context.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
