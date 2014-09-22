#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h> 

/*
The fork() function is used to create a new process from an existing process. The new process is called the child process, and the existing process is called the parent. You can tell which is which by checking the return value from fork(). The parent gets the child's pid returned to him, but the child gets 0 returned to him. 
*/

int main()
{
   pid_t pid;
    
   switch (pid = fork())
   {
   case -1:
       /* Here pid is -1, the fork failed */
       /* Some possible reasons are that you're */
       /* out of process slots or virtual memory */
       printf("The fork failed!");
       exit(-1);
    
   case 0:
       /* pid of zero is the child */
       printf("Child:  hello!\n");
       _exit(0);
    
   default:
       /* pid greater than zero is parent */

       /* wait for child process to end */
       waitpid(-1, NULL, 0);

       printf("Parent: child's pid is %d\n",pid);
   }
}
 

