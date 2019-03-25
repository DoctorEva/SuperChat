#include <stdlib.h>
#include <ncurses.h>
#include <string.h>

#include "SuperChat.h"


int main(int argc, char** argv)
{
  printf("<dev> Main called\n");
  Client_Window A; 
  A.GUI_main(); //Begins the GUI
  
  return 0;
}
