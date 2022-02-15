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
	/* Initialize input buffer */
	char buff[BUFFER_SIZE];

	printf ("shell> ");

	/* Ingest input from stdin up to BUFFER_SIZE characters */
	fgets(buff, sizeof(buff), stdin);
	buff[strcspn(buff, "\n")] = 0; // Gets rid of stdin newline character
	
	/* Tokenize input arguments */
	char* args[MAX_ARGS];
	const char* delimiters = "\t \n";
	char* token = strtok(buff, delimiters);
	int i = 0;
	while (token && i < MAX_ARGS)
	{
		args[i] = token;
		token = strtok(NULL, delimiters);
		++i;
	}
	args[i] = NULL; // i will be one element past the last input argument
	
	pid_t pid = fork();

  	if (pid < 0)
  	{
    	fprintf (stderr, "fork error (%s) -- exiting\n",
	    	strerror (errno));
    	exit (-1);
  	}
	/* Child process */
	if (pid == 0)
  	{
		printf ("[ %s ] (PID: %d)\n", args[0], getpid());
		int ret = execvp (args[0], args);
		if (ret < 0)
		{
			fprintf (stderr, "exec error (%s) -- exiting\n", strerror (errno));
			exit(1);
		}
	       	exit(0);
 	}
	/* Parent process */
	int status;
	pid_t wpid = waitpid(pid, &status, 0);
	if (wpid < 0)
	{
		printf ("waitpid error");
	}
	else if (WIFEXITED(status))
	{
		printf ("[* %s *] (Exit: %d)\n", args[0], WEXITSTATUS(status));
	}

	return 0;
}
