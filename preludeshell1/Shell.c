// Author: Mike Wilson
// Prelude to a Shell: Part 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

const int BUFFER_SIZE = 80;

int
main ()
{
	char buff[BUFFER_SIZE];

	printf ("shell> ");

	fgets(buff, sizeof(buff), stdin);
	buff[strcspn(buff, "\n")] = 0;

	__pid_t pid = fork();

  	if (pid < 0)
  	{
    	fprintf (stderr, "fork error (%s) -- exiting\n",
	    	strerror (errno));
    	exit (1);
  	}
	if (pid == 0)
  	{
		/* Since this is the child process, the ppid is going to be the same as the pid of the parent */
    	printf("Child says:  PID = %d, parent PID = %d, '%s'\n", getpid(), getppid(), buff);
    	exit (0); 
 	}
	/* 
	 * Since this is the parent process, the ppid will be the same as the pid of the process
	 * that called this whole program. In this case that is the currently running instance of
	 * bash that was used to execute this program.
	 */
	printf("Parent says: PID = %d, child PID = %d, parent PID = %d\n", getpid(), pid, getppid());

	return 0;
}
