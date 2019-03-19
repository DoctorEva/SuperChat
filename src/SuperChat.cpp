// Main Function for SuperChat
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>

#include "SuperChat.h"

void Client_Window::display_Login()
{
  printf("Display login Window Called\n");
}

int main(int argc, char** argv)
{
  printf("Main called\n");
  Client_Window A;
  A.display_Login();
  return 0;
}
