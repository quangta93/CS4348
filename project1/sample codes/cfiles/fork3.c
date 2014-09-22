#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
 

int main()
{
   pid_t pid;
   pid_t ppid;
   pid_t chpid;

   int i;
   for (i=0; i<3; i++)
   {
      switch (pid = fork())
      {
         case -1:
	     
	    /* Here pid is -1, the fork failed */
	    /* Some possible reasons are that you're */
	    /* out of process slots or virtual memory */
	    perror("The fork failed!");
		break;
			      
	 
	 case 0:
	    /* pid of zero is the child */
	    ppid = getppid();
	    chpid = getpid();
	    printf("Hi, I'm child #%d, my pid is %d, my parent pid is %d\n", i, chpid, ppid);
	    _exit(0);
						   
	 default:
	    /* pid greater than zero is parent getting the child's pid */
	    waitpid(-1, NULL, 0);
      }
   }

   ppid = getpid();
   printf("%d: Hi, I'm the parent, my pid is %d\n",i, ppid);

   printf("Done\n", ppid);
}
						       

