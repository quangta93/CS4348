/* Ozbirn, 09/17/13
   Demonstrates redirecting child's stdin and stdout.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main()
{
   char value;
   int pipe1[2];
   int pipe2[2];

   if (pipe(pipe1) < 0 || pipe(pipe2) < 0)
   {
      write(STDERR_FILENO, "Pipe failed\n", 12);
      exit(1);
   }

   switch(fork())
   {
     case -1: 
      write(STDERR_FILENO, "Fork failed\n", 12);
      exit(2);

     case 0: 
      dup2(pipe1[0], STDIN_FILENO);   /* repl stdin with pipe1[0]  */
      dup2(pipe2[1], STDOUT_FILENO);  /* repl stdout with pipe2[1] */
      close(pipe1[0]);                /* close these               */
      close(pipe1[1]);
      close(pipe2[0]);
      close(pipe2[1]);
      execlp("rw", "rw", (char *) 0); /* run rw program            */
      write(STDERR_FILENO, "Exec failed\n", 12);
      exit(3);

     default:
      value = 'A';
      printf("Sent %c\n", value);     /* print it                  */
      write(pipe1[1], &value, 1);     /* send "A" to child         */
      read(pipe2[0], &value, 1);      /* read what child sends     */
      printf("Read %c\n", value);     /* print it                  */
   }
}

