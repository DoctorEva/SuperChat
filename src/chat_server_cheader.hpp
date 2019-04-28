//
// chat_server.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <utility>
#include "asio.hpp"
#include "chat_message.hpp"

using asio::ip::tcp;

//----------------------------------------------------------------------

typedef std::deque<chat_message> chat_message_queue;

//----------------------------------------------------------------------

class chat_participant
{
public:
  virtual ~chat_participant() {}
  virtual void deliver(const chat_message& msg) = 0;
};

typedef std::shared_ptr<chat_participant> chat_participant_ptr;

//----------------------------------------------------------------------

class chat_room
{
public:
  void join(chat_participant_ptr participant)
  {
    participants_.insert(participant);
    std::cout<<"A user has joined!"<<std::endl; //Remove
    for (auto msg: recent_msgs_)
      participant->deliver(msg);
  }

  void leave(chat_participant_ptr participant)
  {
    participants_.erase(participant);
    std::cout<<"User has left :("<<std::endl; //Remove
  }

  void deliver(const chat_message& msg)
  {
    recent_msgs_.push_back(msg);
    while (recent_msgs_.size() > max_recent_msgs)
      recent_msgs_.pop_front();

    for (auto participant: participants_)
      participant->deliver(msg);
  }

  char* getName()
  {
    return chatroomName;
  }

  void setName(char* chatRoomName)
  {
	chatroomName = chatRoomName;
  }

private:
  std::set<chat_participant_ptr> participants_;
  enum { max_recent_msgs = 100 };
  chat_message_queue recent_msgs_;
  char *chatroomName;
};

//----------------------------------------------------------------------
class student_chatroom
{
public:
  std::vector<std::string> messages;
  std::vector<std::string> cur_users;
private:
  std::string name;
  student_chatroom(std::string Name)
  {
    name = Name;
  }
};
std::vector<std::string> ALLUSERS;
std::vector<student_chatroom> CHATROOMS;

class chat_session
  : public chat_participant,
    public std::enable_shared_from_this<chat_session>
{
public:
  chat_session(tcp::socket socket, chat_room& room)
    : socket_(std::move(socket)),
      room_(room)
  {
  }

  void start()
  {
    room_.join(shared_from_this());
    do_read_header();
  }

  void deliver(const chat_message& msg)
  {
    bool write_in_progress = !write_msgs_.empty();
    write_msgs_.push_back(msg);
    if (!write_in_progress)
    {
      do_write();
    }
  }

private:
  void do_read_header()
  {
    auto self(shared_from_this());
    asio::async_read(socket_,
        asio::buffer(read_msg_.data(), chat_message::header_length),
        [this, self](std::error_code ec, std::size_t /*length*/)
        {
          if (!ec && read_msg_.decode_header())
          {
            do_read_body();
          }
          else
          {
            room_.leave(shared_from_this());
          }
        });
  }

  void do_read_body()
  {
    auto self(shared_from_this());
    asio::async_read(socket_,
        asio::buffer(read_msg_.body(), read_msg_.body_length()),
        [this, self](std::error_code ec, std::size_t /*length*/)
        {
          if (!ec)
          {
            read_msg_.body()[read_msg_.body_length()] = '\0';
	          std::cout<<"Received: '"<<read_msg_.body()<<"'"<<std::endl;

            char body[read_msg_.body_length()+1];
            strcpy(body, read_msg_.body());
            char* tok = strtok(body, "-");
            char arg[read_msg_.body_length()+1];
            if(tok)
            {
              strcpy(arg, &(read_msg_.body()[strlen(tok)+1]));
              if(!strcmp(tok, "NEW_USER"))
              {
                printf("Adding new user %s\n", arg);
                ALLUSERS.push_back(arg);
                unsigned int i;
                for(i = 0; i < ALLUSERS.size(); i++)
                {
                  std::cout<<ALLUSERS[i]<<std::endl;
                }
              }
              else if(!strcmp(tok, "LOGOFF"))
              {
                printf("Logging off user %s\n", arg);
                std::string argstr = arg;
                ALLUSERS.erase( std::remove(begin(ALLUSERS), end(ALLUSERS), argstr), end(ALLUSERS));
              }
              else if(!strcmp(tok, "NEW_ROOM"))
              {
                printf("Attempting to create %s\n", arg);
              }
              else if(!strcmp(tok, "DEL_ROOM"))
              {
                printf("Attempting to delete %s\n", arg);
              }
              else if(!strcmp(tok, "GET"))
              {
                chat_message msg;
                char bod[] = "GETSTART";
                msg.body_length(strlen(bod));
                memcpy(msg.body(), bod, msg.body_length()+1);
                msg.encode_header();
                room_.deliver(msg);

                if(!strcmp(arg, "SHARED"))
                {
                  chat_message msg;
                  char bod[] = "END";
                  msg.body_length(strlen(bod));
                  memcpy(msg.body(), bod, msg.body_length()+1);
                  msg.encode_header();
                  room_.deliver(msg);
                }
                else if(!strcmp(arg, "USERS"))
                {
                  chat_message msg;
                  char bod[] = "END";
                  msg.body_length(strlen(bod));
                  memcpy(msg.body(), bod, msg.body_length()+1);
                  msg.encode_header();
                  room_.deliver(msg);
                }
                else if(!strcmp(arg, "ALLUSERS"))
                {
                  unsigned int i;
                  chat_message msg;
                  for(i=0; i<ALLUSERS.size(); i++)
                  {
                    char* bod = strdup(ALLUSERS[i].c_str());
                    msg.body_length(strlen(bod));
                    memcpy(msg.body(), bod, msg.body_length()+1);
                    msg.encode_header();
                    room_.deliver(msg);
                  }
                  char bod[] = "END";
                  msg.body_length(strlen(bod));
                  memcpy(msg.body(), bod, msg.body_length()+1);
                  msg.encode_header();
                  room_.deliver(msg);
                }
                else if(!strcmp(arg, "MSSGS"))
                {
                  chat_message msg;
                  char bod[] = "END";
                  msg.body_length(strlen(bod));
                  memcpy(msg.body(), bod, msg.body_length()+1);
                  msg.encode_header();
                  room_.deliver(msg);
                }
                else if(!strcmp(arg, "CHATROOMS"))
                {
                  chat_message msg;
                  char bod[] = "END";
                  msg.body_length(strlen(bod));
                  memcpy(msg.body(), bod, msg.body_length()+1);
                  msg.encode_header();
                  room_.deliver(msg);
                }
                else
                {
                  chat_message msg;
                  char bod[] = "END";
                  msg.body_length(strlen(bod));
                  memcpy(msg.body(), bod, msg.body_length()+1);
                  msg.encode_header();
                  room_.deliver(msg);
                }
              }
            }


            //room_.deliver(read_msg_);
            do_read_header();
          }
          else
          {
            room_.leave(shared_from_this());
          }
        });
  }

  void do_write()
  {
    auto self(shared_from_this());
    asio::async_write(socket_,
        asio::buffer(write_msgs_.front().data(),
          write_msgs_.front().length()),
        [this, self](std::error_code ec, std::size_t /*length*/)
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
            room_.leave(shared_from_this());
          }
        });
  }

  tcp::socket socket_;
  chat_room& room_;
  chat_message read_msg_;
  chat_message_queue write_msgs_;

};


//----------------------------------------------------------------------

class chat_server
{
public:
  chat_server(asio::io_context& io_context,
      const tcp::endpoint& endpoint)
    : acceptor_(io_context, endpoint)
  {
    do_accept();
  }

private:
  void do_accept()
  {
    acceptor_.async_accept(
        [this](std::error_code ec, tcp::socket socket)
        {
          if (!ec)
          {
            std::make_shared<chat_session>(std::move(socket), room_)->start();
          }

          do_accept();
        });
  }

  tcp::acceptor acceptor_;
  chat_room room_;
};

//----------------------------------------------------------------------
