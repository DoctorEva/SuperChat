// Implementations for SuperChat GUI, using ncurses.
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <iostream>

#include <string>
#include <cstring>
#include <fstream>
#include "SuperChat.h"
#include "chat_client_cheader.hpp"
#include <ncurses.h>

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

// Display an initial prompt to the user, asking them to enter a nickname.
void Client_Window::display_Login()
{
  // Formatting the window.
  initscr();
  printf("<dev> Display login Window Called\n");
  WINDOW *Login_Window;
  int startx, starty, width, height;
  char input_name[NICKNAME_CHARS];
  int num_users = 0; // NOT IMPLEMENTED HERE - to be taken from Server. // num_users = current_server.get_num_users()
  height = 20;
  width = 80;
  starty = (LINES - height) / 2;
  startx = (COLS - width) / 2;
  Login_Window = newwin(height, width, starty, startx);
  box(Login_Window, 0, 0);
  int ch;
  char mssg1[] = "WELCOME TO SUPERCHAT";
  char mssg2[] = "Current Users: ";
  char mssg3[] = "Please enter a nickname!";
  mvwprintw(Login_Window,3,((width-strlen(mssg1))/2),"%s",mssg1);
  mvwprintw(Login_Window,5,((width-strlen(mssg2)-3)/2), "%s%d/%d",mssg2, num_users, MAX_USERS);

  // Take an input nickname from the user and confirm it
  while(ch != 121 ) // Allow the user to keep attempting names until they confirm an untaken name.
  {
    mvwprintw(Login_Window,7, 3, "%s",mssg3);
    wrefresh(Login_Window);
    wmove(Login_Window, 8, 3);
    wclrtoeol(Login_Window);
    echo();
    input_name[0] = '\0';
    // Read in the input name, which cannot be empty.
    while(input_name[0]=='\0')
    {
      mvwgetstr(Login_Window, 8, 3, input_name);
    }
    input_name[NICKNAME_CHARS] = '\0'; // Truncates the string to the max num of characters.
    if(send_login_request(input_name))
    {
      wmove(Login_Window, 9, 3);
      wclrtobot(Login_Window);
      box(Login_Window, 0, 0);
      mvwprintw(Login_Window, 9, 3, "Confirm \"%s\"? y to continue, any key to redo.", input_name);
      noecho();
      wrefresh(Login_Window);
      ch = wgetch(Login_Window);
    }
    else
    {
      mvwprintw(Login_Window, 9, 3, "Username is taken. Please try again!");
      wrefresh(Login_Window);
    }
  }
  mvwprintw(Login_Window, 10, 3,"Welcome, \"%s\". Attempting to join server..", input_name);
  wrefresh(Login_Window);

  // The user has successfully joined with a unique username, free up the screen.
  delwin(Login_Window);
  endwin();
  strcpy(username, input_name); // Save the user's accepted nickname in the class attribute.
}

// This screen allows you to navigate through a chatroom and it's features.
void Client_Window::display_Chatroom()
{
  // Initialize the display_Chatroom screen and format it.
  initscr();
  printf("<dev> Display Chatroom called\n");
  mvprintw(0,0, "F1 : Switch to Chatroom Select\n"); // TODO - Add the chatroom name.
  mvprintw(1,0, "LEFT or RIGHT to switch tabs. UP and DOWN to scroll.\n");
  int ch = 0; // Character input. This is your "keybinds"
  enum tab{CHAT, USERS, SHARED, BLACKLIST};
  int tab = CHAT; // What tab is the user looking at currently.
  WINDOW* Chatroom_Window;
  Chatroom_Window = newwin(17,75, 3, 0);
  box(Chatroom_Window, 0,0);
  int chat_offset = 0; // Used to allow the user to scroll through chat.
  int x;
  std::vector<std::string> temp; // TEMP
  temp.push_back(username); // TEMP
  temp.push_back("This is an example");
  temp.push_back("Find Hot Singles Near You!");
  // Run through what the chatroom window does until you switch to Chatroom Selection window
  while(ch != 80) //  On F1 pressed.
  {
    // Clear the subwindow
    werase(Chatroom_Window);
    box(Chatroom_Window, 0, 0);
    wrefresh(Chatroom_Window);
    // Clear the input line (below the subwindow)
    move(20,0);
    clrtobot();
    noecho();
    refresh();
    //mvwprintw(Chatroom_Window,1,1, "<dev> This is a new window, %s! %d  %d %d ", username, ch, tab, chat_offset);
    wrefresh(Chatroom_Window);

    switch(tab)
    {
      case CHAT:
        mvprintw(2,0, "CHAT | Users | Shared Files | Blacklist");
        mvprintw(20,0, ">> Press Enter to begin typing");
        refresh();
        refresh_chat(Chatroom_Window, chat_offset);
        ch = getch();
        switch(ch)
        {
          case 10: // On enter
            echo();
            char* input_mssg;
            input_mssg = (char*)malloc(sizeof(char)*100); // Tempoary, memory allocation handled by others.
            move(20,3);
            clrtoeol();
            refresh();
            mvgetstr(20, 3, input_mssg);
            send_message_to_chat(input_mssg); // Sends message to Chatroom for handling.
            break;
          case 65: // On up arrow
            //Increment Chat offset to let the user scroll up
            chat_offset ++;
            x = 50; // Temp, should be set to the current number of messages in the chatroom.
            if(chat_offset > x)
              chat_offset = x;
            break;
          case 66: // On Down Arrow
            chat_offset = 0;
            break;
        }
        break;
      case USERS:
        mvprintw(2,0, "Chat | USERS | Shared Files | Blacklist");
        mvprintw(20, 0, " e - blacklist selected ");
        refresh();
        x = temp.size(); // Temp, should be set to the current number of users in the chatroom.
        if(x>0)
        {
          refresh_list_tab(Chatroom_Window, chat_offset, temp);
        }
        else
        {
          mvwprintw(Chatroom_Window, 1, 1, "No users present! How are you here?..");
          wrefresh(Chatroom_Window);
        }
        ch = getch();

        if(x > 0)
        {
          switch(ch)
          {
            case 101: // On e
              //TODO - add the selected name to the blacklist.
              break;
            case 66: // On Dowm Arrow
              chat_offset ++;
              chat_offset = chat_offset % x;
              break;
            case 65: // On Up Arrow
              chat_offset = 0;
              break;
          }
        }
        break;
      case SHARED:
        mvprintw(2,0, "Chat | Users | SHARED FILES | Blacklist");
        mvprintw(20, 0, " e - download selected");
        refresh();
        x = temp.size(); // Temp, should be set to the current number of files in the chatroom
        if(x>0)
        {
          refresh_file_tab(Chatroom_Window, chat_offset, temp);
        }
        else
        {
          mvwprintw(Chatroom_Window, 1, 1, "No shared files.");
          wrefresh(Chatroom_Window);
        }
        ch = getch();
        if(x>0)
        {
          switch(ch)
          {
            case 101: // on e
              // TODO - Send the required info to the server to copy the file selected.
              break;
            case 99: // On c pressed, attempt to upload a new file.
              move(21,0);
              char input_name[100];
              printw("New chat name: \n");
              echo();
              input_name[0] = '\0';
              while(input_name[0] == '\0')
              {
                getstr(input_name);
                move(22,0);
                clrtoeol();
              }
              move(21,0);
              clrtoeol();
              noecho();
              if(send_upload_request(input_name))
              {

              }
              else
              {
                printw("Sorry, '%s' is taken.", input_name);
              }
              break;
            case 66: // on Down arrow
              chat_offset ++;
              x = temp.size();
              chat_offset = chat_offset % x;
              break;
            case 65: // On Up arrow
              chat_offset = 0;
              break;
          }
        }
        break;
      case BLACKLIST:
        mvprintw(2,0, "Chat | Users | Shared Files | BLACKLIST");
        mvprintw(20, 0, " e - unban selected");
        refresh();
        std::vector<std::string> entries = read_file("~SuperChat");
        x = entries.size();
        if(x>0)
        {
          refresh_blacklist_tab(Chatroom_Window, chat_offset, entries);
        }
        else
        {
          mvwprintw(Chatroom_Window, 1, 1, "No blacklisted users.");
          wrefresh(Chatroom_Window);
        }
        ch = getch();
        if(x>0)
        {
          switch(ch)
          {
            case 101: // on e
              remove_blacklist(entries[chat_offset]);
              chat_offset = 0;
              break;
            case 66: // on down arrow
              chat_offset ++;
              chat_offset = chat_offset % x;
              break;
            case 65: // On up arrow
              chat_offset = 0;
              break;
          }
        }
        break;
    }
    // Handle switching tabs if the user entered Left or Right.
    if(ch == 68) // On Left Arrow, switch tabs.
    {
      tab--;
      if(tab < 0)
      {
        tab += 4;
      }
      chat_offset = 0;
    }
    if(ch == 67) // On Right Arrow, switch tabs.
    {
      tab++;
      tab = tab % 4;
      chat_offset = 0;
    }
  }
  // Switching to Chatroom Select, free the screen.
  werase(Chatroom_Window);
  wrefresh(Chatroom_Window);
  erase();
  refresh();
  delwin(Chatroom_Window);
  endwin();
}

// Displays the Chatroom Select window, returns false if the user signed out.
int Client_Window::display_ChatroomSelect()
{
  // Initialize the chatroom select screen and format it.
  printf("<dev> display ChatroomSelect called.\n");
  initscr();
  mvprintw(0,0, "F1 : Switch to Chatroom Window");
  mvprintw(1,0, "UP and DOWN to select a chatroom");
  mvprintw(20,0, " r - Signout | e - join chatroom | q - delete chatroom | c - create room");
  int ch = 0;
  int selection_offset = 0;
  WINDOW* Select_Window;
  Select_Window = newwin(17,75,3,0);
  box(Select_Window, 0,0);
  wrefresh(Select_Window);
  refresh();
  int x;

  // Allow the chatroom select window to run until the user switches to a chatroom window.
  while(ch != 80) // On F1 pressed, switch back to chatroom window.
  {
    std::vector<std::string> temp; // TEMP
    temp.push_back("Lobby"); // TEMP
    temp.push_back("ExampleRoom"); // TEMP
    refresh_chatselect(Select_Window, selection_offset, temp);
    switch(ch)
    {
      case 114: // on r pressed, signout and exit the program
        delwin(Select_Window);
        endwin();
        send_signoff_to_server();
        printf("Exiting by signout\n");
        return 0;
        break;
      case 101: // on e pressed, switch to the selected chatroom
        break;
      case 113: // On q pressed, delete the selected chatroom IF it is empty.
        break;
      case 99: // On c pressed, attempt to create a new chatroom
        move(21,0);
        char input_name[100];
        printw("New chat name: \n");
        echo();
        input_name[0] = '\0';
        while(input_name[0] == '\0')
        {
          getstr(input_name);
          move(22,0);
          clrtoeol();
        }
        move(21,0);
        clrtoeol();
        noecho();
        if(send_chatroom_create(input_name))
        {

        }
        else
        {
          printw("Sorry, '%s' is taken.", input_name);
        }
        break;
      case 66: // on down arrow pressed
        x = temp.size(); // Temp, X should be set to the # of chatrooms on the server.
        selection_offset ++;
        selection_offset = selection_offset % x;
        break;
      case 65: // on up arrow pressed
        selection_offset --;
        if(selection_offset < 0)
          selection_offset = 0;
        break;
    }
    noecho();
    ch = getch();
  }
  // Switching to the Chatroom window, free the screen.
  delwin(Select_Window);
  endwin();
  return 1;
}

void Client_Window::add_blacklist(char* banning_target)
{
  FILE *saves = fopen("~SuperChat", "a");
  if(saves)
  {
    fwrite(banning_target, 1, strlen(banning_target), saves);
    fputc('\n', saves);
    fclose(saves);
  }
}
void Client_Window::remove_blacklist(std::string removal_target)
{
  std::vector<std::string> entries = read_file("~SuperChat");
  FILE* output = fopen("~SuperChat", "w");
  fclose(output);
  int i;
  for(i=0; i<entries.size(); i++)
  {
    if(entries[i].compare(removal_target))
    {
      char* addition = new char [(entries[i]+"\n").length()+1];
      std::strcpy(addition, entries[i].c_str());
      add_blacklist(addition);
    }
  }
}
void Client_Window::refresh_chat(WINDOW* chatwindow, int offset)
{
  int pos_y = 1;
  int pos_x = 1;
  int i;

  std::vector<std::string> messages = read_file("Lobby");

  for(i=offset; i<messages.size() && pos_y < 15; i++)
  {
    mvwprintw(chatwindow, pos_y, 1, "%s : %s\n", username, messages[messages.size() - i - 1].c_str());
    getyx(chatwindow, pos_y, pos_x);
  }
  box(chatwindow,0,0);
  wrefresh(chatwindow);
}
void Client_Window::refresh_chatselect(WINDOW* chatselectwindow, int offset, std::vector<std::string> chatroom_names)
{
  int i;
  for (i=0; i<chatroom_names.size(); i++)
  {
    wmove(chatselectwindow, i+1, 0);
    wprintw(chatselectwindow,"-- %s\n", chatroom_names[i].c_str());
  }
  box(chatselectwindow,0,0);
  wmove(chatselectwindow, offset+1, 0);
  wrefresh(chatselectwindow);
}
void Client_Window::refresh_blacklist_tab(WINDOW* chatwindow, int offset, std::vector<std::string> entries)
{
  int i;
  for(i=0; i<entries.size(); i++)
  {
    wmove(chatwindow, i+1, 0);
    wprintw(chatwindow,"-- %s\n", entries[i].c_str());
  }
  box(chatwindow, 0, 0);
  wmove(chatwindow, offset+1, 0);
  wrefresh(chatwindow);
}
void Client_Window::refresh_list_tab(WINDOW* chatwindow, int offset, std::vector<std::string> usernames)
{
  int i;
  for(i=0; i<usernames.size(); i++)
  {
    wmove(chatwindow, i+1, 0);
    wprintw(chatwindow,"-- %s\n", usernames[i].c_str());
  }
  box(chatwindow, 0, 0);
  wmove(chatwindow, offset+1, 0);
  wrefresh(chatwindow);
}
void Client_Window::refresh_file_tab(WINDOW* chatwindow, int offset, std::vector<std::string> server_files)
{
  int i;
  for(i=0; i<server_files.size(); i++)
  {
    wmove(chatwindow, i+1, 0);
    wprintw(chatwindow,"-- %s\n", server_files[i].c_str());
  }
  box(chatwindow, 0, 0);
  wmove(chatwindow, offset+1, 0);
  wrefresh(chatwindow);
}
// send_login_request
// Sends a name to the client to determine if it is taken.
// Returns 0 if Taken, or 1 if Free.
int Client_Window::send_login_request(char* name)
{
  int success = 1;
  // TODO - Scan through all online users' names on the server. return 0 if a match is found.
  return success;
}
void Client_Window::send_download_request(char* filename)
{
  //TODO - Find the filename on the server, read that file and place it in the user's current working directory.
}
int Client_Window::send_upload_request(char* filename)
{
  int success = 0;
  //TODO - Read the file from the user's current working directory, save it on the server.
  // Return 1 if success, or 0 if failed.
  return success;
}
void Client_Window::send_signoff_to_server()
{
  //TODO - Remove the client from the server.
}
void Client_Window::send_chatroom_delete(int index)
{
  // TODO - Delete the requested chatroom at index if it is empty.
}
int Client_Window::send_chatroom_create(char* name)
{
  int success = 0;
  // TODO - Create a new chatroom with the given name. Return 1 if successful.
  return success;
}
void Client_Window::send_message_to_chat(char* input)
{
  chat_message msg;
  msg.body_length(strlen(input));
  memcpy(msg.body(), input, msg.body_length());
  msg.encode_header();
  c->write(msg);
}

void Client_Window::GUI_main(chat_client* Lobby)
{
  //S = server;
  c = Lobby;
  username = (char*) malloc(NICKNAME_CHARS*sizeof(char));      // Allocating memory for the username.
  keypad(stdscr, TRUE);                            // Allows GUI to use function keys, ect.
  display_Login();                                 // Begin prompting for a nickname and join the server.
  // TODO - set current_chatroom to Lobby chatroom and current_server to Server. May need to pass in as parameters.
  int x = 1;
  while(x)                                         // Allows the user to alternate between these two windows
  {
    display_Chatroom();                          // Start displaying the SuperChat Lobby.
    x = display_ChatroomSelect();                    // Allows the user to change lobby, ends the program when they sign out.
  }
}