// Implementations for SuperChat GUI, using ncurses.
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>

#include "SuperChat.h"

void Client_Window::display_Login()
{
  initscr();
  printf("<dev> Display login Window Called\n");
  WINDOW *Login_Window;
  int startx, starty, width, height;
  char input_name[NICKNAME_CHARS];
  int num_users = 0; // NOT IMPLEMENTED HERE - to be taken from Server.
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
  while(ch != 121) // On 'y'
    {
      mvwprintw(Login_Window,7, 3, "%s",mssg3);
      wrefresh(Login_Window);
      wmove(Login_Window, 8, 3);
      wclrtoeol(Login_Window);
      echo();
      mvwgetstr(Login_Window, 8, 3, input_name);
      input_name[NICKNAME_CHARS] = '\0'; // Truncates the string. // TODO - Code for when username is taken.
      mvwprintw(Login_Window, 9, 3, "Confirm \"%s\"? y to continue, any key to redo.", input_name);
      cbreak();
      noecho();
      ch = wgetch(Login_Window);
      //wprintw(Login_Window, "%d read from the keyboard, hopefully %c will be taken.", ch, ch);
    }
  mvwprintw(Login_Window, 10, 3,"Welcome, \"%s\". Attempting to join server..", input_name);
  wrefresh(Login_Window);
  /*
    TODO - Add user to a join server wait queue, continue when they are added.
    TODO - If the user enters q at any time while waiting to join, quit the program
   */
  wborder(Login_Window,' ',' ',' ',' ',' ',' ',' ',' ');
  wrefresh(Login_Window);
  delwin(Login_Window);
  endwin();
  strcpy(username, input_name); // Save the user's accepted nickname in the class attribute.
}
void Client_Window::display_Chatroom()
{
  initscr();
  printf("<dev> Display Chatroom called\n");
  mvprintw(0,0, "F1 : Switch to Chatroom Select\n"); // TODO - Add the chatroom name.
  mvprintw(1,0, "LEFT or RIGHT to switch tabs. UP and DOWN to scroll.\n");
  int ch = 0;
  int tab = 0;
  WINDOW* Chatroom_Window;
  Chatroom_Window = newwin(17,75, 3, 0);
  box(Chatroom_Window, 0,0);
  int chat_offset = 0; // Allow the user to scroll through chat.
  while(ch != 80) //  On F1 pressed.
    {
      move(20,0);
      clrtoeol();
      refresh();
      mvwprintw(Chatroom_Window,1,1, "<dev> This is a new window, %s! %d  %d %d ", username, ch, tab, chat_offset);
      if(tab == 0) // On the CHAT tab
	{
	  mvprintw(2,0, "CHAT | Users | Shared Files | Blacklist");
	  mvprintw(20,0, ">> Press Enter to begin typing");
	  refresh();
	  refresh_chat(Chatroom_Window); // Not implemented yet.
	  wrefresh(Chatroom_Window);
	  
	  ch = getch();
	  if(ch == 10) // On Enter
	    {
	      echo();
	      char input_mssg[200];
	      move(20,3);
	      clrtoeol();
	      refresh();
	      mvgetstr(20, 3, input_mssg);
	      mvwprintw(Chatroom_Window, 2,1,"<dev> %s: %s", username, input_mssg);
	      send_message_to_chat(input_mssg); // Sends message to Chatroom for handling.
	      refresh_chat(Chatroom_Window);
	    }
	  else if(ch == 65) // On UP arrow
	    {
	      //Increment Chat offset to let the user scroll up
	      chat_offset ++; // May need to be limited to the # of messages total.
	      refresh_chat(Chatroom_Window, chat_offset);
	    }
	  else if(ch == 66) // On DOWN arrow
	    {
	      chat_offset = 0; // Returns the user to the most recent message.
	      refresh_chat(Chatroom_Window, chat_offset);
	    }
	}
      else if(tab == 1) // On the USERS tab
	{
	  mvprintw(2,0, "Chat | USERS | Shared Files | Blacklist");
	  refresh();

	  ch = getch();
	}
      else if(tab == 2) // On the Shared Files tab
	{
	  mvprintw(2,0, "Chat | Users | SHARED FILES | Blacklist");
	  refresh();

	  ch = getch();
	}
      else if(tab == 3) // On the Blacklist tab
	{
	  mvprintw(2,0, "Chat | Users | Shared Files | BLACKLIST");
	  refresh();

	  ch = getch();
	}
      
      if(ch == 68) // On Left Arrow, switch tabs.
	{
	  tab--;
	  if(tab < 0)
	    {
	      tab += 4;
	    }
	}
      if(ch == 67) // On Right Arrow, switch tabs.
	{
	  tab++;
	  tab = tab % 4;
	}
    }
  werase(Chatroom_Window);
  wrefresh(Chatroom_Window);
  erase();
  refresh();
  delwin(Chatroom_Window);
  endwin();
}
void Client_Window::display_ChatroomSelect()
{
  printf("<dev> display ChatroomSelect called.\n");
  initscr();
  mvprintw(0,0, "F1 : Switch to Chatroom Window");
  mvprintw(1,0, "UP and DOWN to select a chatroom");
  mvprintw(20,0, " r - Signout ");
  int ch = 0;
  int selection_offset = 0;
  WINDOW* Select_Window;
  Select_Window = newwin(17,75,3,0);
  box(Select_Window, 0,0);
  wrefresh(Select_Window);
  refresh();

  while(ch != 80) // On F1 pressed,
    {
      if(ch == 114) // On r pressed
	{
	  delwin(Select_Window);
	  endwin();
	  send_chatroom_delete();
	  printf("Exiting by signout\n");
	  // TODO - Sign the user out of the Server.
	  exit(0);
	}
      else if(ch == 65) // On UP pressed
	{

	}
      else if(ch == 66) // On DOWN pressed
	{

	}
      noecho();
      ch = getch();
    }
  delwin(Select_Window);
  endwin();
}

void Client_Window::add_blacklist()
{

}
void Client_Window::remove_blacklist()
{

}
void Client_Window::send_message_to_chat(char* message)
{

}
void Client_Window::refresh_chat(WINDOW* chatwindow, int offset)
{

}
void Client_Window::send_download_request()
{

}
void Client_Window::send_upload_request(std::string filename)
{

}
void Client_Window::send_signoff_to_server()
{

}
void Client_Window::send_chatroom_delete()
{

}

void Client_Window::GUI_main()
{
  username = (char*) malloc(NICKNAME_CHARS*sizeof(char));      // Allocating memory for the username.
  keypad(stdscr, TRUE);                            // Allows GUI to use function keys, ect.
  display_Login();                                 // Begin prompting for a nickname and join the server.
  // TODO - set current_chatroom to Lobby chatroom and current_server to Server. May need to pass in as parameters.
  while(1)                                         // Allows the user to alternate between these two windows
    {
      display_Chatroom();                          // Start displaying the SuperChat Lobby.
      display_ChatroomSelect();                    // Allows the user to change lobby, ends the program when they sign out.
    }
}
