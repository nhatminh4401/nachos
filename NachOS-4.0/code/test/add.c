/* add.c
 *	Simple program to test whether the systemcall interface works.
 *	
 *	Just do a add syscall that adds two values and returns the result.
 *
 */

#include "syscall.h"

int main()
{
  int result;
  char c;
  char str[100];
  result = Add(42, 23);
  //  c = ReadChar();
  //  PrintChar(c);
  ReadString(str, 100);
  PrintString(str);
  Halt();
  /* not reached */
}
