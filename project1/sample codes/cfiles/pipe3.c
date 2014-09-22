/* p. 509, A Book on C, by Al Kelley and Ira Pohl */

/*Uses pipes to sum N rows concurrently.*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define N 3

int  add_vector(int v[]);
void error_exit(char *s);

int main()
{
   int a[N][N] = {{1, 1, 1}, {2, 2, 2}, {3, 3, 3}},
      i, row_sum, sum =0,
      pd[2];                              /* pipe descriptors */

   if (pipe(pd) == -1)                    /* create a pipe */
      error_exit("pipe() failed");

   for (i=0; i < N; i++)
   {
      if (fork() == 0)                    /* child process */
      {                    
         row_sum = add_vector(a[i]);
         if (write(pd[1], &row_sum, sizeof(int)) == -1)
            error_exit("write() failed");
         _exit(0);                       /* exit from child */
      }
   }

   for (i = 0;  i < N; ++i) 
   {
      if (read(pd[0], &row_sum, sizeof(int)) == -1)
         error_exit("read() failed");
      sum += row_sum;
   }

   for (i = 0;  i < N; ++i) 
      waitpid(-1, NULL, 0);

   printf("Sum of the array = %d\n", sum);

   return 0;
}

int add_vector(int v[])
{
   int i, vector_sum=0;

   for (i = 0; i < N;  ++i)
      vector_sum += v[i];

   return vector_sum;
}


void error_exit(char *s)
{
   fprintf(stderr,"\nERROR: %s - bye!\n", s);
      exit(1);
}

