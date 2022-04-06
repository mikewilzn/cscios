/*
 * Filename	: mysystem.c
 * Author	: Michael Wilson
 * Course	: CSCI 380
 * Assignment	: Redirection
 * Description	: Implement Linux "system" function
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

void
mysystem (char* command);

int
main (int argc, char** argv)
{
	if (argc != 2)
	{
		printf("Usage: %s <command>\n", argv[0]);
		exit(0);
	}
	mysystem(argv[1]);
}

void
mysystem (char* command)
{
	pid_t pid = fork();

	if (pid < 0)
	{
		fprintf(stderr, "fork error (%s) -- exiting\n", strerror (errno));
		exit(1);
	}
	if (pid == 0)
	{
		int fd = open("System.out", O_WRONLY | O_CREAT | O_TRUNC, 0600);
		if (fd < 0)
		{
			printf("Unable to create file \"System.out\"\n");
			exit(1);
		}
		dup2(fd, STDOUT_FILENO); // Redirect stdout to opened file
		close(fd);

		int ret = execl("/bin/sh", "sh", "-c", command, (char *) NULL);
		if (ret < 0)
		{
			fprintf(stderr, "exec error (%s) -- exiting\n", strerror (errno));
			exit(1);
		}
	}
	int status;
	pid_t child = waitpid(pid, &status, 0);

	if (child == pid)
	{
		if (WIFEXITED(status))
		{
			printf("Exit status: %d\n", WEXITSTATUS(status));
		}
		else if (WIFSIGNALED(status))
		{
			printf("Exit status: %d\n", WTERMSIG(status));
		}
	}
	return;
}
