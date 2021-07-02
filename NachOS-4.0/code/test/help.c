/* help.c
 *	Simple program to introduce some basics about our team and briefly discription about the sort and acsii program.
 */

#include "syscall.h"

int main()
{
  PrintString("!!!!!!!This is our NachOS 4.0 project!!!!!!!\n");
  PrintString("Our team members: Nguyen Cao Thien Long - ID: 19127203\n");
  PrintString("                  Doan Nhat Minh - ID: 19127469\n");
  PrintString("And now , we will introduce some system call in Nachos 4.0 throught two example: ");
  PrintString("1.Ascii example : This example is about to use some system call as PrintNum,PrintChar... ");
  PrintString("                  to print all readable character in Ascii table");
  PrintString("2.Sort example :  Like example 1, this example also use system call to Sort array of number ");
  PrintString("                  to ascending order or descending order by Bubble Sort Algorithm");
  PrintString("That is all about our project.We hope that it will please you");
  PrintString("Thank you for reading :3");
  Halt();
  /* not reached */
}