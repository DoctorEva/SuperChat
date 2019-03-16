#ifndef SUPERCHAT_H
#define SUPERCHAT_H

#include <ncurses.h>
#include <stdlib.h>

#define MAX_USERS 50

class Chatroom
{

}
class Server
{

}

class Client_Window
{
  Chatroom* current_chatroom;
  Server* current_server;
  std::string username;
 public:
  void GUI_main(); // The handler for the GUI thread
  void set_chatroom(Chatroom* room);
  void display_Login(); // Displays the login screen
  void display_ChatroomSelect();
  void display_Chatroom();

  void add_blacklist(); // Opens Superchat~ and adds an entry
  void remove_blacklist();// Opens Superchat~ and deletes a specific entry

  void send_message_to_chat(std::string message); // sends a message to a chatroom
  void send_download_request(); // calls Download from the chatroom
  void send_upload_request(std::string filename); // Sends a filename to Chatroom for upload
  void send_signoff_to_server(); // Exit the server and ends the program.
  void send_chatroom_delete(); // Tells the chatroom to delete itself.
}
