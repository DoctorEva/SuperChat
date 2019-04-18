#ifndef SUPERCHAT_H
#define SUPERCHAT_H

#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <vector>
#include "chat_client_cheader.hpp"
#include <ncurses.h>

#define MAX_USERS 50
#define NICKNAME_CHARS 20 // To be changed. // Character limit of input for nicknames

class Client_Window
{
  chat_client* c;
  char* username;
  char* secret_msg_code;
  std::vector<std::string> chatRooms;
  std::string currentChatroom;
 public:
  void GUI_main(chat_client* Lobby); // The handler for the GUI thread
  void refresh_x(); // Tells the GUI to refresh whatever window is displaying.

private:
  //void set_chatroom(Chatroom* room);
  void display_Login(); // Displays the login screen
  int display_ChatroomSelect();
  void display_Chatroom();

  void add_blacklist(char* banning_target); // Opens Superchat~ and adds an entry
  void remove_blacklist(std::string removal_target);// Opens ~SuperChat and deletes a specific entry
  void refresh_chat(WINDOW* chatwindow, int offset);
  void refresh_chatselect(WINDOW* chatselectwindow, int offset);
  void refresh_blacklist_tab(WINDOW* chatwindow, int offset, std::vector<std::string> entries);
  void refresh_list_tab(WINDOW* chatwindow, int offset, std::vector<std::string> usernames);
  void refresh_file_tab(WINDOW* chatwindow, int offset, std::vector<std::string> server_files);

  int send_login_request(char* name);
  void send_message_to_chat(char* message); // sends a message to a chatroom. Depends on Chatroom.
  void send_download_request(char* filename); // calls Download from the chatroom. Depends on Chatroom
  int send_upload_request(char* filename); // Sends a filename to Chatroom for upload. Depends on Chatroom
  void send_signoff_to_server(); // Exit the server and ends the program. Depends on Server.
  void send_chatroom_delete(int index); // Tells the chatroom to delete itself. Depends on Server.
  int send_chatroom_create(char* name); // Sends a request to make a new chatroom.
};


#endif
