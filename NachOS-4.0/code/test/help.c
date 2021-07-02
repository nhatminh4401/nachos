/* help.c
 *	Simple program to introduce some basics about our team and briefly discription about the sort and acsii program.
 */

#include "syscall.h"

int main()
{
  PrintString("!!!!!!!This is our NachOS 4.0 project!!!!!!!\n");
  PrintString("Our team members: Handsome Nguyen Cao Thien Long Captain - ID: 19127203\n");
  PrintString("                  Doan Nhat Minh - ID: 19127469\n");
  Halt();
  /* not reached */
}