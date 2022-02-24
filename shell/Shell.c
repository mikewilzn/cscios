/*
 * Filename	: Shell.c
 * Author	: Michael Wilson
 * Course	: CSCI 380-01
 * Assignment	: Prelude to a shell 3
 * Description	: Tiny shell program
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <signal.h>

void evaluate (char* command);
void tokenize (char* args[], char* command);
void handler (int signal);

const int BUFFER_SIZE = 80;
const int MAX_ARGS = 5;

int
main ()
{
	if (signal (SIGCHLD, handler) == SIG_ERR)
	{
		fprintf (stderr, "signal error (%s) -- exiting\n", strerror (errno));
		exit (1);
	}

	while (1)
	{
		/* Initialize input buffer */
		char buff[BUFFER_SIZE];
		printf ("shell> ");

		/* Ingest input from stdin up to BUFFER_SIZE characters */
		fgets(buff, sizeof(buff), stdin);

		if(feof(stdin))
			break;

		evaluate(buff);
	}	
	return 0;
}

void
evaluate (char* command)
{
	char* args[MAX_ARGS];
	tokenize(args, command);

	/* Handle whitespace input */
	if(args[0] == NULL)
		return;

	/* Exit when user types 'exit' */
	if(strcmp(args[0], "exit") == 0)
		exit(0);

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
		printf ("[ PID: %d ]\n", getpid());
		int ret = execvp (args[0], args);
		if (ret < 0)
		{
			fprintf (stderr, "exec error (%s) -- exiting\n", strerror (errno));
			exit(1);
		}
 	}

	int delay = 2;
	while ((delay = sleep(2)) > 0);
}

void
tokenize (char* args[], char* command)
{	
	const char* delimiters = "\t \n";
	char* token = strtok(command, delimiters);
	int i = 0;
	while (token && i < MAX_ARGS - 1)
	{
		args[i] = token;
		token = strtok(NULL, delimiters);
		++i;
	}
	args[i] = NULL; // i will be one element past the last input argument
}

void
handler (int signal)
{
	int tempErrNo = errno;

	int status;
	pid_t wpid;

	while ((wpid = waitpid(-1, &status, WNOHANG)) > 0)
	{
		if (WIFEXITED(status))
		{
			printf ("[ PID: %d (Exit: %d) ]\n", wpid, WEXITSTATUS(status));
		}
		else if (WIFSIGNALED(status))
		{
			printf ("[ PID: %d (Signal: %d) ]\n", wpid, WTERMSIG(status));
		}
	}

	errno = tempErrNo;
}
