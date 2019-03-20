// Implementations for SuperChat
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>

#include "SuperChat.h"

void Client_Window::display_Login()
{
  printf("<dev> Display login Window Called\n");
  WINDOW *Login_Window;
  int startx, starty, width, height;
  char input_name[50];
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
  mvwprintw(Login_Window, 8, 3, "                                                                             ");
  echo();
  mvwgetstr(Login_Window, 8, 3, input_name);
  mvwprintw(Login_Window, 9, 3, "Confirm \"%s\"? y to continue, any key to redo.", input_name);
  cbreak();
  noecho();
  ch = wgetch(Login_Window);
  //wprintw(Login_Window, "%d read from the keyboard, hopefully %c will be taken.", ch, ch);
  //wgetch(Login_Window);
    }
  mvwprintw(Login_Window, 10, 3,"Welcome, \"%s\". Attempting to join server..", input_name);
  wrefresh(Login_Window);
  /*
    TODO - Add user to a join server wait queue, then add them to the server.
   */
  //wgetch(Login_Window);
  delwin(Login_Window);
}
void Client_Window::display_Chatroom()
{
  
}
void Client_Window::GUI_main()
{
  initscr();
  printw("This is some text.\n");
  display_Login();
  display_Chatroom();
  endwin();
}

int main(int argc, char** argv)
{
  printf("<dev> Main called\n");
  Client_Window A;
  A.GUI_main();
  
  return 0;
}
