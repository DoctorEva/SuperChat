// Implementations for SuperChat
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
  char input_name[30];
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
  while(ch != 121)
    {
  mvwprintw(Login_Window,7, 3, "%s",mssg3);
  wrefresh(Login_Window);
  mvwprintw(Login_Window, 8, 3, "                                                                           ");
  echo();
  mvwgetstr(Login_Window, 8, 3, input_name);
  mvwprintw(Login_Window, 9, 3, "Confirm \"%s\"? y to continue, any key to redo.", input_name);
  cbreak();
  noecho();
  ch = wgetch(Login_Window);
  //wprintw(Login_Window, "%d read from the keyboard, hopefully %c will be taken.", ch, ch);
    }
  mvwprintw(Login_Window, 10, 3,"Welcome, \"%s\". Attempting to join server..", input_name);
  wrefresh(Login_Window);
  /*
    TODO - Add user to a join server wait queue, then add them to the server.
    TODO - If the user enters q, quit the program.
   */
  wborder(Login_Window,' ',' ',' ',' ',' ',' ',' ',' ');
  wrefresh(Login_Window);
  delwin(Login_Window);
  endwin();
  strcpy(username, input_name); // Save the user's accepted nickname!
}
void Client_Window::display_Chatroom()
{
  initscr();
  printf("<dev> Display Chatroom called\n");
  mvprintw(0,0, "F1 : Switch to Chatroom Select\n"); // TODO - Add the chatroom name.
  int ch = 0;
  int tab = 0;
  WINDOW* Chatroom_Window;
  Chatroom_Window = newwin(17,75, 3, 0);
  box(Chatroom_Window, 0,0);
  
  while(ch != 80) // 80 = KEY_F(1)
    {
      if(tab == 0)
	{
	  mvprintw(2,0, "CHAT | Users | Shared Files | Blacklist");
	  mvprintw(20,0, ">> Press Enter to begin typing");
	  refresh();
	  // TODO - Update the chat window with messages.
	  mvwprintw(Chatroom_Window,1,1, "<dev> This is a new window, %s! %d  %d  ", username, ch, tab);
	  wrefresh(Chatroom_Window);
	  //noecho();
	  ch = getch();
	}
      else if(tab == 1)
	{
	  mvprintw(2,0, "Chat | USERS | Shared Files | Blacklist");
	  mvprintw(20,0, ">> Press Enter to begin typing");
	  refresh();

	  ch = getch();
	}
      else if(tab == 2)
	{
	  mvprintw(2,0, "Chat | Users | SHARED FILES | Blacklist");
	  mvprintw(20,0, ">> Press Enter to begin typing");
	  refresh();

	  ch = getch();
	}
      else if(tab == 3)
	{
	  mvprintw(2,0, "Chat | Users | Shared Files | BLACKLIST");
	  mvprintw(20,0, ">> Press Enter to begin typing");
	  refresh();

	  ch = getch();
	}
      if(ch == 68) // On Left Arrow
	{
	  tab--;
	  if(tab < 0)
	    {
	      tab += 4;
	    }
	}
      if(ch == 67) // On Right Arrow
	{
	  tab++;
	  tab = tab % 4;
	}
    }
  delwin(Chatroom_Window);
  endwin();
}
void Client_Window::display_ChatroomSelect()
{
  printf("<dev> display ChatroomSelect called.\n");
  exit(0); // Temp, should be done during signout.
}
void Client_Window::GUI_main()
{
  username = (char*) malloc(30*sizeof(char)); // Allocating memory for a username.
  display_Login();
  while(1)
    {
      display_Chatroom(); // Start displaying the SuperChat Lobby.
      display_ChatroomSelect();
    }
}

int main(int argc, char** argv)
{
  printf("<dev> Main called\n");
  keypad(stdscr, TRUE);
  Client_Window A;
  A.GUI_main();
  
  return 0;
}
