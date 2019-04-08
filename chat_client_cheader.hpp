//
// chat_client_header.hpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef CHAT_CLIENT_HPP
#define CHAT_CLIENT_HPP

#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>
#include <string.h>
#include <stdio.h>
#include <fstream>
#include "asio.hpp"
#include "chat_message.hpp"
#include <ncurses.h>

using asio::ip::tcp;

typedef std::deque<chat_message> chat_message_queue;

class chat_client
{
public:
  chat_client(asio::io_context& io_context,
      const tcp::resolver::results_type& endpoints)
    : io_context_(io_context),
      socket_(io_context)
  {
    do_connect(endpoints);
  }

  void write(const chat_message& msg,std::string fileName, WINDOW* chatWindow,char* userName, int offset)
  {
    filename=fileName; //Assigns a file name to write to for persistant chat log
    chatwindow = chatWindow; //Assigns window
    username = userName; //Assigns client username
    asio::post(io_context_,
        [this, msg]()
        {
          bool write_in_progress = !write_msgs_.empty();
          write_msgs_.push_back(msg);
          if (!write_in_progress)
          {
	    std::ofstream fout;
            std::string line;
	    fout.open(filename, std::ios::app);

	    //std::cout<<"This is a message being sent"<< std::endl; //Remove
            //std::cout.write(read_msg_.body(), read_msg_.body_length());
            //std::cout << "\n\r";
	    fout<<read_msg_.body()<<std::endl; //print message to file.
	    fout.close();
	    //assert(!fout.fail());
            do_write();
          }
        });
  }

  void close()
  {
    asio::post(io_context_, [this]() { socket_.close(); });
  }

private:
  void do_connect(const tcp::resolver::results_type& endpoints)
  {
    asio::async_connect(socket_, endpoints,
        [this](std::error_code ec, tcp::endpoint)
        {
          if (!ec)
          {
            do_read_header();
          }
        });
  }

  void do_read_header()
  {
    asio::async_read(socket_,
        asio::buffer(read_msg_.data(), chat_message::header_length),
        [this](std::error_code ec, std::size_t /*length*/)
        {
          if (!ec && read_msg_.decode_header())
          {
	    
            do_read_body();
          }
          else
          {
            socket_.close();
          }
        });
  }

  void do_read_body()
  {
    asio::async_read(socket_,
        asio::buffer(read_msg_.body(), read_msg_.body_length()),
        [this](std::error_code ec, std::size_t /*length*/)
        {
          if (!ec)
          {
	    int pos_y = 1;
	    int pos_x = 1;
	    int i;

	    std::vector<std::string> messages = read_file(filename);

	    for(i=chat_offset; i<messages.size() && pos_y < 15; i++)
	    {
	      mvwprintw(chatwindow, pos_y, 1, "%s : %s\n", username, messages[messages.size() - i - 1].c_str());
	      getyx(chatwindow, pos_y, pos_x);
	    }
	    box(chatwindow,0,0);
	    wrefresh(chatwindow);
            do_read_header();
          }
          else
          {
            socket_.close();
          }
         
        });
  }

  void do_write()
  {
    asio::async_write(socket_,
        asio::buffer(write_msgs_.front().data(),
          write_msgs_.front().length()),
        [this](std::error_code ec, std::size_t /*length*/)
        {
          if (!ec)
          {
            write_msgs_.pop_front();
            if (!write_msgs_.empty())
            {
              do_write();
            }
          }
          else
          {
            socket_.close();
          }
        });
  }

// Reads in a file and returns it's contents in a vector.
std::vector<std::string> read_file(std::string filename)
{
  std::vector<std::string> all;
  std::ifstream input;
  std::string line;
  input.open(filename);

  if(!input.is_open())
    {
      std::cout<<"File failed to load"<<std::endl;
      //exit(1);
    }
  while(!input.eof())
    {
      //input>>line;
      std::getline(input,line);
      all.push_back(line);
    }
  all.pop_back();
  input.close();
  return all;
}

private:
  asio::io_context& io_context_;
  tcp::socket socket_;
  chat_message read_msg_;
  chat_message_queue write_msgs_;
  std::string filename;
  WINDOW* chatwindow;
  char* username;
  int chat_offset;
};

#endif // CHAT_CLIENT_HPP

