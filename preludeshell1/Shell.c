// Author: Mike Wilson
// Prelude to a Shell: Part 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

int main ()
{
	const int CHAR_LIMIT = 80;

	const char *buff[] = malloc(CHAR_LIMIT * sizeof(char));

	printf ("cmd>");

	__pid_t cpid = fork();
	__pid_t ppid = getpid()


	char *fgets(buff, CHAR_LIMIT, stdin);
	printf(buff);

	printf("Parent says: PID = %d, child PID = %d, parent PID = %d\n");
	printf("Child says:  PID = %d, parent PID = %d, '%s'\n");
}
