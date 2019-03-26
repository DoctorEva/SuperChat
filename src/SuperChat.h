#ifndef SUPERCHAT_H
#define SUPERCHAT_H

#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#define MAX_USERS 50
#define NICKNAME_CHARS 20 // To be changed. // Character limit of input for nicknames

class Client_Window
{
  //Chatroom* current_chatroom; // TODO - point to Chatroom class
  //Server* current_server; // TODO - point to the Server class
  char* username;
 public:
  void GUI_main(); // The handler for the GUI thread
  //void set_chatroom(Chatroom* room);
  void display_Login(); // Displays the login screen
  void display_ChatroomSelect();
  void display_Chatroom();

  void add_blacklist(); // Opens Superchat~ and adds an entry
  void remove_blacklist();// Opens Superchat~ and deletes a specific entry
  void refresh_chat(WINDOW* chatwindow, int offset = 0);
  void refresh_chatselect(WINDOW* chatselectwindow, int offset = 0);
  void refresh_blacklist_tab(WINDOW* chatwindow, int offset = 0);
  void refresh_list_tab(WINDOW* chatwindow, int offset = 0);
  void refresh_file_tab(WINDOW* chatwindow, int offset = 0);

  void send_message_to_chat(char* message); // sends a message to a chatroom. Depends on Chatroom.
  void send_download_request(); // calls Download from the chatroom. Depends on Chatroom
  void send_upload_request(std::string filename); // Sends a filename to Chatroom for upload. Depends on Chatroom
  void send_signoff_to_server(); // Exit the server and ends the program. Depends on Server.
  void send_chatroom_delete(); // Tells the chatroom to delete itself. Depends on Server.
};


#endif
