/* Ozbirn, 09/17/13
   Reads a char, increments it, and prints it.
   For use with program wr.c.
*/

#include <stdio.h>

int main()
{
   char value;

   scanf("%c", &value);
   value++;
   printf("%c", value);

   return 0;
}
