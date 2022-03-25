/*
 * tsh - A tiny shell program
 *
 * I tried to keep the SIGINT and SIGTSTP handlers as simple as possible and let
 * SIGCHLD handler handle the print statements. Both of the former handlers just
 * call kill using the "-n" option which automatically sends the respective signal
 * to all processes in the process group (with g_runningPid being the leader).
 *
 * SIGCHLD calls waitpid with the "-1" option which waits for every child process.
 * I also passed the WNOHANG because otherwise any suspended process would cause
 * the shell to hang. I passed WUNTRACED because without it my shell was hanging
 * and looking at the man page I suspected some child processes were not being
 * traced.
 *
 * waitfg waits for a foreground process using sigsuspend while the runningPid
 * isn't zero. Because once it's complete it will be set to zero.
 */

/*
 *******************************************************************************
 * INCLUDE DIRECTIVES
 *******************************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>


#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/*
 *******************************************************************************
 * TYPE DEFINITIONS
 *******************************************************************************
 */

typedef void handler_t (int);

/*
 *******************************************************************************
 * PREPROCESSOR DEFINITIONS
 *******************************************************************************
 */

// max line size 
#define MAXLINE 1024 
// max args on a command line 
#define MAXARGS 128

/*
 *******************************************************************************
 * GLOBAL VARIABLES
 *******************************************************************************
 */

// defined in libc
extern char** environ;   

// command line prompt 
char prompt[] = "tsh> ";

// for composing sprintf messages
char sbuf[MAXLINE];

// PID of the foreground job's leader, or 0 if there is no foreground job
volatile pid_t g_runningPid = 0;
// PID of the suspended job's leader, or 0 if there is no suspended job
volatile pid_t g_suspendedPid = 0; 

sigset_t mask, prev;
/*
 *******************************************************************************
 * FUNCTION PROTOTYPES
 *******************************************************************************
 */

bool
parseline (const char* cmdline, char**argv);

void
eval (char* cmdline);

bool
builtin_cmd();

void
waitfg();

void
sigint_handler (int sig);

void
sigtstp_handler (int sig);

void
sigchld_handler (int sig);

void
sigquit_handler (int sig);

void
unix_error (char* msg);

void
app_error (char* msg);

handler_t*
Signal (int signum, handler_t* handler);

/*
 *******************************************************************************
 * MAIN
 *******************************************************************************
 */

int
main (int argc, char** argv)
{
	/* Redirect stderr to stdout */
	dup2 (1, 2);

	/* Install signal handlers */
	Signal (SIGINT, sigint_handler);   /* ctrl-c */
	Signal (SIGTSTP, sigtstp_handler); /* ctrl-z */
	Signal (SIGCHLD, sigchld_handler); /* Terminated or stopped child */
	Signal (SIGQUIT, sigquit_handler); /* quit */

	while (true)
	{
		char buff[MAXLINE];
		printf(prompt);
		fflush(stdout);

		fgets(buff, sizeof(buff), stdin);

		if(feof(stdin))
			break;

		eval(buff);

		fflush(stdout);
	}

	/* Quit */
	exit (0);
}

/*
 *  parseline - Parse the command line and build the argv array.
 *
 *  Characters enclosed in single quotes are treated as a single
 *  argument.
 *
 *  Returns true if the user has requested a BG job, false if
 *  the user has requested a FG job.
 */
bool
parseline (const char* cmdline, char** argv)
{
	static char array[MAXLINE]; /* holds local copy of command line*/
	char* buf = array;          /* ptr that traverses command line*/
	char* delim;                /* points to first space delimiter*/
	int argc;                   /* number of args*/
	bool bg;                     /* background job?*/

	strcpy (buf, cmdline);
	buf[strlen (buf) - 1] = ' ';  /* replace trailing '\n' with space*/
	while (*buf && (*buf == ' ')) /* ignore leading spaces*/
		buf++;

	/* Build the argv list*/
	argc = 0;
	if (*buf == '\'')
	{
		buf++;
		delim = strchr (buf, '\'');
	}
	else
	{
		delim = strchr (buf, ' ');
	}

	while (delim)
	{
		argv[argc++] = buf;
		*delim = '\0';
		buf = delim + 1;
		while (*buf && (*buf == ' ')) /* ignore spaces*/
			buf++;

		if (*buf == '\'')
		{
			buf++;
			delim = strchr (buf, '\'');
		}
		else
		{
			delim = strchr (buf, ' ');
		}
	}
	argv[argc] = NULL;

	if (argc == 0) /* ignore blank line*/
		return true;

	/* should the job run in the background?*/
	if ((bg = (*argv[argc - 1] == '&')) != false)
	{
		argv[--argc] = NULL;
	}
	return bg;
}

/*
 * eval - Runs command input by user.
 *
 * Runs parseline to tokenize input buffer into an array of arguments
 * Blocks signals while reading/setting global variables and then
 * unblocks to run command.
 *
 * If the command is recognized as a built-in command it is run
 * by the current process. If not, a child process is created
 * with fork() which then runs the supplied command.
 *
 * Supports running process in background if a '&' is present
 * at the end of the input buffer.
 */
void
eval (char* cmdline)
{
	char* argv[MAXARGS];    // Argument list
	char buf[MAXLINE];    // Holds modified command line
	int bg;   // Should job run in bg or fg?

	strcpy(buf, cmdline);
	bg = parseline(buf, argv);

	if (argv[0] == NULL)
		return;

	sigemptyset (&mask);
	sigaddset (&mask, SIGCHLD);
	sigprocmask (SIG_BLOCK, &mask, &prev);

	if (!builtin_cmd(argv))
	{
		/* Child runs job */
		g_runningPid = fork();
		if (g_runningPid < 0)
		{
			fprintf (stderr, "fork error (%s) -- exiting\n",
					strerror (errno));
			exit (1);
		}
		if (g_runningPid == 0)
		{
			setpgid(0, 0);
			sigprocmask(SIG_SETMASK, &prev, NULL);

			if (execvp(argv[0], argv) < 0)
			{
				printf("%s: Command not found\n", argv[0]);
				exit(0);
			}
			waitfg();
		}

		sigprocmask(SIG_SETMASK, &prev, NULL); // Unblock SIGCHLD

		if (!bg)
		{
			waitfg();
		}
		else
		{
			printf("(%d) %s", g_runningPid, cmdline);
			fflush(stdout);
		}
	}

	return;
}

/*
 * builtin_cmd - Checks to see if given command is a built in shell command
 * and runs it if it is. 
 *
 * fg - brings suspended process back to the foreground and locks shell.
 *
 * Returns TRUE if given command is built in.
 * FALSE if it is not a built in command.
 */
bool
builtin_cmd (char** argv)
{
	/* Exit shell when user types 'exit' */
	if (strcmp(argv[0], "quit") == 0)
		exit(0);

	if (strcmp(argv[0], "fg") == 0)
	{
		if (g_suspendedPid > 0)
		{
			g_runningPid = g_suspendedPid;
			g_suspendedPid = 0;

			sigprocmask (SIG_SETMASK, &prev, NULL);
			kill(-g_runningPid, SIGCONT);
			waitfg();
		}
		return true;
	}
	return false;
}

/*
 * waitfg - Indefinitely waits for the running pid to exit and be set to zero.
 * While it's waiting, the shell is locked via sigsuspend.
 */
void
waitfg ()
{
	while (g_runningPid != 0)
	{
		sigsuspend (&prev);
	}
}

/*
 *******************************************************************************
 * SIGNAL HANDLERS
 *******************************************************************************
 */

/*
 *  sigchld_handler - The kernel sends a SIGCHLD to the shell whenever
 *      a child job terminates (becomes a zombie), or stops because it
 *      received a SIGSTOP or SIGTSTP signal. The handler reaps all
 *      available zombie children, but doesn't wait for any other
 *      currently running children to terminate.
 */
void
sigchld_handler (int sig)
{
	int olderrno = errno;

	int status;
	pid_t wpid;

	while ((wpid = waitpid(-1, &status, WNOHANG|WUNTRACED)) > 0)
	{
		if (WIFSIGNALED(status))
		{
			printf ("Job (%d) terminated by signal %d\n", wpid, WTERMSIG(status));
		}
		else if (WIFSTOPPED(status))
		{
			printf ("Job (%d) stopped by signal %d\n", wpid, WSTOPSIG(status));
			g_suspendedPid = g_runningPid;
		}
		g_runningPid = 0;
	}

	errno = olderrno;
	return;
}

/*
 *  sigint_handler - The kernel sends a SIGINT to the shell whenever the
 *     user types ctrl-c at the keyboard.  Catch it and send it along
 *     to the foreground job.
 */
void
sigint_handler (int sig)
{
	if (g_runningPid > 0)
		kill(-g_runningPid, SIGINT);  
	return;
}

/*
 *  sigtstp_handler - The kernel sends a SIGTSTP to the shell whenever
 *      the user types ctrl-z at the keyboard. Catch it and suspend the
 *      foreground job by sending it a SIGTSTP.
 */
	
void
sigtstp_handler (int sig)
{
	if (g_runningPid > 0)
		kill(-g_runningPid, SIGTSTP);

	return;
}

/*
 *******************************************************************************
 * HELPER ROUTINES
 *******************************************************************************
 */


/*
 * unix_error - unix-style error routine
 */
void
unix_error (char* msg)
{
	fprintf (stdout, "%s: %s\n", msg, strerror (errno));
	exit (1);
}

/*
 *  app_error - application-style error routine
 */
void
app_error (char* msg)
{
	fprintf (stdout, "%s\n", msg);
	exit (1);
}

/*
 *  Signal - wrapper for the sigaction function
 */
handler_t*
Signal (int signum, handler_t* handler)
{
	struct sigaction action, old_action;

	action.sa_handler = handler;
	sigemptyset (&action.sa_mask); /* block sigs of type being handled*/
	action.sa_flags = SA_RESTART;  /* restart syscalls if possible*/

	if (sigaction (signum, &action, &old_action) < 0)
		unix_error ("Signal error");
	return (old_action.sa_handler);
}


/*
 *  sigquit_handler - The driver program can gracefully terminate the
 *     child shell by sending it a SIGQUIT signal.
 */
void
sigquit_handler (int sig)
{
	printf ("Terminating after receipt of SIGQUIT signal\n");
	exit (1);
}
