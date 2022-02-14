// Author: Mike Wilson
// Prelude to a Shell: Part 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

const int BUFFER_SIZE = 80;
const int MAX_ARGS = 5;

int
main ()
{
	char buff[BUFFER_SIZE];

	printf ("shell> ");

	fgets(buff, sizeof(buff), stdin);
	buff[strcspn(buff, "\n")] = 0;
	
	char* args[MAX_ARGS];

	const char* delimiters = " \t \n";
	char* token = strtok(buff, delimiters);


	int i = 0;
	while (token && i < MAX_ARGS - 1)
	{
		args[i] = token;
		token = strtok(NULL, delimiters);
		++i;
	}
	args[i] = NULL;
	
	pid_t pid = fork();

  	if (pid < 0)
  	{
    	fprintf (stderr, "fork error (%s) -- exiting\n",
	    	strerror (errno));
    	exit (-1);
  	}
	if (pid == 0)
  	{
		printf ("[ %s ] (PID: %d)\n", args[0], pid);
		int ret = execvp (args[0], args);
		if (ret < 0)
		{
			fprintf (stderr, "exec error (%s) -- exiting\n", strerror (errno));
			exit(1);
		}
	       	exit(0);
 	}
	int status;
	if (waitpid(pid, &status, 0) < 0)
	{
		printf ("waitpid error");
	}
	else if (WIFEXITED(status))
	{
		printf ("[* %s *] (Exit: %d)\n", args[0], WEXITSTATUS(status));
	}

	return 0;
}
