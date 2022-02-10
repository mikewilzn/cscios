// Demonstrate use of fork, exec, and wait

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// For strerror
#include <string.h>

// For fork, getpid, execve, read, write, close,
//   and other Linux system calls
#include <unistd.h>
// For waitpid
#include <sys/wait.h>
// For errno
#include <errno.h>

int
main ()
{
  printf ("Parent: before fork\n"); 
  pid_t pid = fork ();
  if (pid < 0)
  {
    fprintf (stderr, "fork error (%s) -- exiting\n",
	     strerror (errno));
    exit (-1);
  }
  if (pid == 0)
  {
    printf ("Child: hi\n");
    char* args[] = { "ls", "-l", NULL };
    int ret = execvp (args[0], args); 
    if (ret < 0)
    {
      fprintf (stderr, "exec error (%s) -- exiting\n",
	       strerror (errno));
      exit (1);
    }
  }
  printf ("Parent: waiting on child %d\n", pid);
  // Invoke waitpid with correct arguments
  // assert that the return value of waitpid matches 'pid'
  //   (you will need a header file for assert)
  // Print the child's exit status
  // ...
  
  return 0;
}
