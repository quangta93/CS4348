/* from Unix book:  "Your Unix, The Ultimate Guide", by Das */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main()
{
   int fd[2];

   if (pipe(fd) < 0)
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
      close(fd[0]);
      dup2(fd[1], STDOUT_FILENO);
      close(fd[1]);
      execlp("cat", "cat", "/etc/passwd", (char *) 0);
      write(STDERR_FILENO, "Exec failed\n", 12);
      exit(3);

     default:
      close(fd[1]);
      dup2(fd[0], STDIN_FILENO);
      close(fd[0]);
      execlp("tr", "tr", "'[a-z]'", "'[A-Z]'", (char *) 0); 
      /* execlp("head", "head", "-5", (char *) 0); */
      write(STDERR_FILENO, "Exec failed\n", 12);
      exit(4);
   }
}

