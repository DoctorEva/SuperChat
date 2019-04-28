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
      endwin();
      std::cout<<"File '"<<filename<<"' failed to load"<<std::endl;
      initscr();
      return all;
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
  //initscr();
  WINDOW *Login_Window;
  int startx, starty, width, height;
  char input_name[NICKNAME_CHARS];
  int num_users = get_from_server("ALLUSERS").size();
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
  wattron(Login_Window, A_BOLD | A_BLINK);
  mvwprintw(Login_Window,3,((width-strlen(mssg1))/2),"%s",mssg1);
  wattroff(Login_Window, A_BOLD | A_BLINK);
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
      mvwgetnstr(Login_Window, 8, 3, input_name, NICKNAME_CHARS);
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
  // Username is free and user is joining, send to server for addition.
  chat_message msg;
  char bod[100];
  sprintf(bod,"NEW_USER-%s", input_name);
  msg.body_length(strlen(bod));
  memcpy(msg.body(), bod, msg.body_length()+1);
  msg.encode_header();
  c->write(msg, currentChatroom, username);

  mvwprintw(Login_Window, 10, 3,"Welcome, \"%s\". Attempting to join server..", input_name);
  wrefresh(Login_Window);

  // The user has successfully joined with a unique username, free up the screen.
  delwin(Login_Window);
  //endwin();
  strcpy(username, input_name); // Save the user's accepted nickname in the class attribute.
}

// This screen allows you to navigate through a chatroom and it's features.
void Client_Window::display_Chatroom(word_search searcher)
{
  // Initialize the display_Chatroom screen and format it.
  //initscr();
  mvprintw(0,0, "F1 : Switch to Chatroom Select. You are in %s\n", currentChatroom.c_str());
  mvprintw(1,0, "LEFT or RIGHT to switch tabs. UP and DOWN to scroll.\n");
  int ch = 0; // Character input. This is your "keybinds"
  enum tab{CHAT, USERS, SHARED, BLACKLIST};
  int tab = CHAT; // What tab is the user looking at currently.
  WINDOW* Chatroom_Window;
  Chatroom_Window = newwin(17,75, 3, 0);
  box(Chatroom_Window, 0,0);
  int chat_offset = 0; // Used to allow the user to scroll through chat.
  int x;
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
    int wait_time = 500;
    timeout(wait_time); // Update screen after .5 secs without input
    switch(tab)
    {
      case CHAT:
        mvprintw(2,0, "CHAT | Users | Shared Files | Blacklist");
        mvprintw(20,0, ">> Press Enter to begin typing.| e - secret code");
        refresh();
        refresh_chat(Chatroom_Window, chat_offset);
        ch = getch();
        switch(ch)
        {
          case 10: // On enter
            echo();
            timeout(-1);
            char input_mssg[512];
            move(20,3);
            clrtoeol();
            refresh();
            mvgetnstr(20, 3, input_mssg, 512);
            // Check for misspelled words.
            {
              char copy[512];
              strcpy(copy, input_mssg);
              char* tok = strtok(copy, " ");
              move(20,3);
              while(tok!=NULL)
              {
                std::string checkword = tok;
                if(!searcher.check_word(checkword))
                  attron(A_UNDERLINE);
                printw("%s", tok);
                attroff(A_UNDERLINE);
                printw(" ");
                tok = strtok(NULL, " ");
                refresh();
              }
              attroff(A_UNDERLINE | A_BOLD);
              mvprintw(23,0, "Type enter to send message. Any other key to cancel.");
              if(getch() == 10)
              {
                sprintf(copy, "%s: %s", username, input_mssg);
                send_message_to_chat(copy); // Sends message to Chatroom for handling
              }

            }
            break;
          case 101: // On e, set a new secret_msg_code
            echo();
            timeout(-1);
            char input_code[20];
            move(20,3);
            clrtoeol();
            refresh();
            mvgetnstr(20, 3, input_code, 20);
            strcpy(secret_msg_code, input_code);
            break;
          case 65: // On up arrow
            //Increment Chat offset to let the user scroll up
            chat_offset ++;
            x = get_from_server("MSSGS").size();
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
        x = get_from_server("USERS").size(); // Temp, should be set to the current number of users in the chatroom.
        if(x>0)
        {
          refresh_list_tab(Chatroom_Window, chat_offset, get_from_server("USERS"));
        }
        else
        {
          mvwprintw(Chatroom_Window, 1, 1, "No users present! How are you here?..");
          wrefresh(Chatroom_Window);
        }
        ch = getch();

        if(x > 0)
        {
          char* selected_str;
          switch(ch)
          {
            case 101: // On e
              selected_str = strdup(get_from_server("USERS")[chat_offset].c_str());
              add_blacklist(selected_str);
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
        mvprintw(20, 0, " e - download selected | c - upload a file");
        refresh();
        x = get_from_server("SHARED").size(); // Temp, should be set to the current number of files in the chatroom
        if(x>0)
        {
          refresh_file_tab(Chatroom_Window, chat_offset, get_from_server("SHARED"));
        }
        else
        {
          mvwprintw(Chatroom_Window, 1, 1, "No shared files.");
          wrefresh(Chatroom_Window);
        }
        ch = getch();
        if(x>0)
        {
          char* selected_str;
          switch(ch)
          {
            case 101: // on e
              selected_str = strdup(get_from_server("SHARED")[chat_offset].c_str());
              send_download_request(selected_str);
              break;
            case 99: // On c pressed, attempt to upload a new file.
              move(21,0);
              char input_file[100];
              printw("Filename to upload: \n");
              echo();
              timeout(-1);
              input_file[0] = '\0';
              while(input_file[0] == '\0')
              {
                getnstr(input_file, 100);
                move(22,0);
                clrtoeol();
              }
              move(22,0);
              clrtoeol();
              noecho();
              if(send_upload_request(input_file))
              {
                printw("Successfully uploaded %s", input_file);
              }
              else
              {
                printw("Sorry, file '%s' could not be sent,", input_file);
              }
              refresh();
              sleep(1);
              break;
            case 66: // on Down arrow
              chat_offset ++;
              x = get_from_server("SHARED").size();
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
  timeout(-1);
  werase(Chatroom_Window);
  wrefresh(Chatroom_Window);
  erase();
  refresh();
  delwin(Chatroom_Window);
  //endwin();
}

// Displays the Chatroom Select window, returns false if the user signed out.
int Client_Window::display_ChatroomSelect()
{
  // Initialize the chatroom select screen and format it.
  //initscr();
  mvprintw(0,0, "F1 : Switch to Chatroom Window");
  mvprintw(0,45, "Current Chatroom: %s", currentChatroom.c_str());
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
    chatRooms = get_from_server("CHATROOMS");
    refresh_chatselect(Select_Window, selection_offset);
    int wait_time = 500;
    timeout(wait_time);
    switch(ch)
    {
      case 114: // on r pressed, signout and exit the program
        delwin(Select_Window);
        endwin();
        send_signoff_to_server();
        printf("Exiting by signout\n");
        return 0;
      case 101: // on e pressed, switch to the selected chatroom
        currentChatroom = chatRooms[selection_offset];
        change_chatroom(selection_offset);
        move(0,45);
        clrtoeol();
        mvprintw(0,45, "Current Chatroom: %s", currentChatroom.c_str());
        refresh();
        break;
      case 113: // On q pressed, delete the selected chatroom IF it is empty.
        if(send_chatroom_delete(selection_offset))
    	  {
          selection_offset = 0;
      	  mvprintw(21,0, "Chatroom successfully deleted.");
	      }
        else
          mvprintw(21,0, "Chatroom is not empty or is Lobby, cannot delete.");
        break;
      case 99: // On c pressed, attempt to create a new chatroom
        move(21,0);
        char input_name[100];
        printw("New chat name: \n");
        echo();
        input_name[0] = '\0';
        timeout(-1);
        while(input_name[0] == '\0')
        {
          getnstr(input_name, 100);
          move(22,0);
          clrtoeol();
        }
        timeout(wait_time);
        move(21,0);
        clrtoeol();
        noecho();
        if(send_chatroom_create(input_name)==1)
        {
          printw("Successfully created %s", input_name);
          mvprintw(0,45, "Current Chatroom: %s", currentChatroom.c_str());
          refresh();
        }
        else if(send_chatroom_create(input_name)==2)
        {
          printw("Sorry, '%s' is taken.", input_name);
        }
      	else
      	{
      	  printw("Maximum Number of Chatrooms reached.");
      	}
        break;
      case 66: // on down arrow pressed
        x = chatRooms.size();
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
  //endwin();
  return 1;
}
void Client_Window::change_chatroom(int index)
{
  char bod[512];
  sprintf(bod, "CHANGE-%d",index);
  chat_message msg;
  msg.body_length(strlen(bod));
  memcpy(msg.body(), bod, msg.body_length()+1);
  msg.encode_header();
  c->write(msg, currentChatroom, username);
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

  std::vector<std::string> messages = get_from_server("MSSGS");
  std::vector<std::string> blacklist = read_file("~SuperChat");
  for(i=offset; i<messages.size() && pos_y < 15; i++)
  {
    //Check if user is on the blacklist.
    int y;
    char* mssg = strdup(messages[messages.size() - i - 1].c_str());
    char* divider = strchr(mssg, ':');
    char name[strlen(mssg) - strlen(divider) + 1];
    memset(name, '\0', strlen(mssg) - strlen(divider) + 1);
    memcpy(name, mssg, strlen(mssg) - strlen(divider));

    for(y=0; y<blacklist.size(); y++)
    {
      if(!strcmp(name, blacklist[y].c_str()))
      {
        break;
      }
    }
    if(y == blacklist.size())
    {
      char* divider = strchr(mssg, ':');
      if(divider[2] == '/') // Check for message secret
      {
        char buffer[strlen(divider)+1];
        strcpy(buffer, divider+3);
        char* token = strtok(buffer, " ");
        if(strcmp(secret_msg_code, token))
        {
          memset(mssg, '#', strlen(mssg));
          wattron(chatwindow, A_DIM);
        }
      }
      mvwprintw(chatwindow, pos_y, 1, "%s\n", mssg);
      wattroff(chatwindow, A_DIM);
      getyx(chatwindow, pos_y, pos_x);
    }
  }
  box(chatwindow,0,0);
  wrefresh(chatwindow);
}
void Client_Window::refresh_chatselect(WINDOW* chatselectwindow, int offset)
{
  wclear(chatselectwindow);
  int i;

  for (i=0; i<chatRooms.size(); i++)
  {
    wmove(chatselectwindow, i+1, 0);
    wprintw(chatselectwindow,"-- %s\n", chatRooms[i].c_str());
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
  std::vector<std::string> allusers = get_from_server("ALLUSERS");
  int i;
  // See if username is taken
  for(i=0;i<allusers.size();i++)
  {
    if(!strcmp(name, allusers[i].c_str()))
      return 0;
  }
  return success;
}
void Client_Window::send_download_request(char* filename)
{
  //TODO - Find the filename on the server, read that file and place it in the user's current working directory.

}
int Client_Window::send_upload_request(char* filename)
{
  int success = 0;
  FILE* input_file;
  input_file = fopen(filename, "r");
  if(input_file)
  {
    // Input file successfully opened. Open the file to write on the server directory. Copy contents of inout file onto server file.
    // TODO
  }
  else
  {
    return 0;
  }
  return success;
}
void Client_Window::send_signoff_to_server()
{
  char mssg[100];
  strcpy(mssg, username);
  strcat(mssg, ": has left the server.");
  send_message_to_chat(mssg);
  //Remove the client from the server.
  chat_message msg;
  char bod[100];
  sprintf(bod,"LOGOFF-%s", username);
  msg.body_length(strlen(bod));
  memcpy(msg.body(), bod, msg.body_length()+1);
  msg.encode_header();
  c->write(msg, currentChatroom, username);
}
int Client_Window::send_chatroom_delete(int index)
{
 int success = 0;

  if(index==0)
  {//Cannot delete lobby.
	return success;
  }
  std::string target_chatroom = chatRooms[index];
  chatRooms.erase( std::remove(begin(chatRooms), end(chatRooms), target_chatroom), end(chatRooms)); // Deleting the chat room
  // TODO - rewrite this so that the server handles deletion.
  std::ofstream fout;
  std::string line;
  fout.open("ChatRooms", std::ios::out);
  for(int i = 0; i<chatRooms.size(); i++)
  {
	fout<<chatRooms[i]<<"\n"; //refresh list
  }
  fout.close();
  chatRooms = read_file("ChatRooms");

  char buf[strlen("./rooms/")+target_chatroom.size()+1] = "./rooms/";
  strcat(buf,target_chatroom.c_str());
  if(!remove(buf))
  {
	success = 1;
	return success;
  }
  else
  {
	return success;
  }
}
int Client_Window::send_chatroom_create(char* name)
{
  int success = 1;
  char bod[512];
  sprintf(bod, "NEW_ROOM-%s",name);
  chat_message msg;
  msg.body_length(strlen(bod));
  memcpy(msg.body(), bod, msg.body_length()+1);
  msg.encode_header();
  c->write(msg, currentChatroom, username);
  char mssg[512];
  change_chatroom(chatRooms.size());
  sprintf(mssg, "%s: <sys> just created the chat.", username);
  send_message_to_chat(mssg);

  return success;
}
void Client_Window::send_message_to_chat(char* input)
{
  char bod[512];
  sprintf(bod, "CHAT-%s",input);
  chat_message msg;
  msg.body_length(strlen(bod));
  memcpy(msg.body(), bod, msg.body_length()+1);
  msg.encode_header();
  c->write(msg, currentChatroom, username);
}

std::vector<std::string> Client_Window::get_from_server(std::string request)
{
  chat_message msg;
  char bod[100];
  //TODO - Send a request to server and get a copy of the vector stored on it.
  // Request vals:
  /*
  SHARED = shared files list.
  USERS = List of users in the current chatroom.
  ALLUSERS = List of all usernames on the server.
  MSSGS = List of messages in the Current chatroom.
  CHATROOMS = list of chatrooms on server
  */
  // Sending Request
  int start = c->update_num;
  sprintf(bod,"GET-%s",request.c_str());
  msg.body_length(strlen(bod));
  memcpy(msg.body(), bod, msg.body_length()+1);
  msg.encode_header();
  c->write(msg, currentChatroom, username);

  while(start == c->update_num);
  return c->ret_vec;
}

void Client_Window::GUI_main(chat_client* Lobby)
{
  username = (char*) malloc(NICKNAME_CHARS*sizeof(char));      // Allocating memory for the username.
  keypad(stdscr, TRUE);                            // Allows GUI to use function keys, ect.
  c = Lobby;
  //Configure presets
  chatRooms= get_from_server("CHATROOMS");
  if(chatRooms.size() != 0)
  {
    currentChatroom = chatRooms[0];
  }
  else
  {
    printf("No connection to server. Exiting SuperChat.\n");
    return;
  }
  word_search searcher("words.txt");
  initscr();
  display_Login();  // Begin prompting for a nickname and join the server.
  secret_msg_code = (char*) malloc(20*sizeof(char));
  memset(secret_msg_code, '\0', 20);
  int x = 1;
  while(x)                                         // Allows the user to alternate between these two windows
  {
    display_Chatroom(searcher);                          // Start displaying the SuperChat Lobby.
    x = display_ChatroomSelect();                    // Allows the user to change lobby, ends the program when they sign out.
  }
  endwin();
  puts("Exiting Superchat by signoff\n");
}
