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

//These includes are needed for the word search
#include <vector>
#include <algorithm>
#include <string>

using asio::ip::tcp;

typedef std::deque<chat_message> chat_message_queue;

class word_search{
  public:

  //when declaring the word_search object, the list is "words.txt"
  word_search(std::string filename){
    //first read file
    std::string line;
    std::ifstream myfile (filename);
    if (myfile.is_open())
    {
      while ( std::getline (myfile,line) )
      {
        word_list.push_back(line);
      }
      myfile.close();
    }
    //sort the list alphebetically
    std::sort(word_list.begin(), word_list.end());
    //hopefully then it's ready for a binary search
  }

  //This function checks the word if it is spelt correctly
  //true is returned if spelt correctly
  //else false is returned
  bool check_word(std::string word_check){
       return binary_search(word_list.begin(), word_list.end(),word_check);
  }

  private:
  std::vector<std::string> word_list;

  //bool sortname(const std::string &lhs, const std::string &rhs) { return *lhs < *rhs; }
};

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

  void write(const chat_message& msg, std::string fileName, char* userName)
  {
    filename = fileName;
    username = userName;
    asio::post(io_context_,
        [this, msg]()
        {
          bool write_in_progress = !write_msgs_.empty();
          write_msgs_.push_back(msg);
          /*
            std::ofstream fout;
            std::string line;
      	    fout.open("./rooms/"+filename, std::ios::app);
      	    fout<<username<< ": "<<msg.body()<<"\n"; //print message to file.
      	    fout.close();
          */
          if (!write_in_progress)
          {
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

private:
  asio::io_context& io_context_;
  tcp::socket socket_;
  chat_message read_msg_;
  chat_message_queue write_msgs_;
  std::string filename;
  char* username;
};

#endif // CHAT_CLIENT_HPP
