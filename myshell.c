#include <errno.h>	// errno()
#include <stdio.h>	// atoi(), execvp(), fflush(), fgets(), printf()
#include <stdlib.h>	// exit()
#include <string.h>	// strcmp(), strerror(), strsignal(), strtok()
#include <sys/types.h>	// wait()
#include <sys/wait.h>	// wait()
#include <unistd.h>	// exit(), fork(), setpgid()

int main (int argc, char** argv){
	char input[4096];
	char* words[100];
	int nwords = 0;;

	while(1){
		int status;

		// ----------
		// READ INPUT
		// ----------
		printf("myshell> ");
		fflush(stdout);
		fgets(input, 100, stdin);
	
		// ------------------
		// PARSE INPUT STRING
		// ------------------
		words[0] = strtok(input, " \t\n");
		nwords++;
		while (words[nwords] = strtok(0, " \t\n"), words[nwords] != NULL)
			nwords++;
		words[nwords] = 0;

		// -------------
		// EXECUTE INPUT	
		// -------------
		if (words[0] == NULL)
			continue;

		// -------------
		// START COMMAND
		// -------------
		else if (strcmp(words[0], "start") == 0){
			if (nwords < 2){
				printf("myshell: not enough arguments to command 'start'\n");
				printf("myshell: usage: start <process_name> <arguments>\n");
			} else {
				pid_t child_pid = fork();
				
				if (child_pid < 0){
					printf("myshell: error: %s\n", strerror(errno));
					exit(EXIT_FAILURE);
				} else if (child_pid == 0){
					setpgid(0, 0);
					setsid();

					if (execvp(words[1], words) < 0){
						printf("myshell: error: %s\n", strerror(errno));
						exit(EXIT_FAILURE);
					}
				} else {
					waitpid(child_pid, &status, WNOHANG | WUNTRACED);
					printf("myshell: process %i started\n", child_pid);
				}
			}

		// ------------
		// WAIT COMMAND
		// ------------
		} else if (strcmp(words[0], "wait") == 0){
			if (nwords != 1){
				printf("myshell: incorrect usage of command 'wait'\n");
				printf("myshell: usage: wait\n");
			} else {
				waitpid(-1, &status, WUNTRACED);
				if (strcmp(strerror(errno), "ECHILD"))
					printf("myshell: no processes left\n");
			}

		// -----------
		// RUN COMMAND
		// -----------
		} else if (strcmp(words[0], "run") == 0){
			if (nwords < 2){
				printf("myshell: not enough arguments to command 'run'\n");
				printf("myshell: usage: run <process_name> <arguments>\n");
			} else {
				pid_t child_pid = fork();
				
				if (child_pid < 0){
					printf("myshell: error: %s\n", strerror(errno));
					exit(EXIT_FAILURE);
				} else if (child_pid == 0){
					if (execvp(words[1], words) < 0){
						printf("myshell: error: %s\n", strerror(errno));
						exit(EXIT_FAILURE);
					}
				} else {
					waitpid(child_pid, &status, 0);
					if (status == 0)
						printf("myshell: process %i exited successfully with status %i\n", child_pid, status);
					else
						printf("myshell: process %i exited abnormally with signal %i - %s\n", child_pid, status, strsignal(status));
				}
			}

		// ------------
		// KILL COMMAND
		// ------------
		} else if (strcmp(words[0], "kill") == 0){
			if (nwords != 2){
				if (nwords < 2)
					printf("myshell: not enough arguments to command 'kill'\n");
				else if (nwords > 2)
					printf("myshell: too many arguments to command 'kill'\n");
				printf("myshell: usage: kill <p_id>\n");
			} else {
				if (kill(atoi(words[1]), SIGKILL) < 0)
					printf("myshell: error: %s\n", strerror(errno));
				else
					printf("myshell: process %s killed\n", words[1]);
			}

		// ------------
		// STOP COMMAND
		// ------------
		} else if (strcmp(words[0], "stop") == 0){
			if (nwords != 2){
				if (nwords < 2)
					printf("myshell: not enough arguments to command 'stop'\n");
				else if (nwords > 2)
					printf("myshell: too many arguments to command 'stop'\n");
				printf("myshell: usage: stop <p_id>\n");
			} else {
				if (kill(atoi(words[1]), SIGSTOP) < 0)
					printf("myshell: error: %s\n", strerror(errno));
				else
					printf("myshell: process %s stopped\n", words[1]);
			}

		
		// ----------------
		// CONTINUE COMMAND
		// ----------------
		} else if (strcmp(words[0], "continue") == 0){
			if (nwords != 2){
				if (nwords < 2)
					printf("myshell: not enough arguments to command 'continue'\n");
				else if (nwords > 2)
					printf("myshell: too many arguments to command 'continue'\n");
				printf("myshell: usage: continue <p_id>\n");
			} else {
				if (kill(atoi(words[1]), SIGCONT) < 0)
					printf("myshell: error: %s\n", strerror(errno));
				else
					printf("myshell: process %s continued\n", words[1]);
			}

		// -----------------
		// QUIT/EXIT COMMAND
		// -----------------
		} else if (strcmp(words[0], "quit") == 0 || strcmp(words[0], "exit") == 0)
			exit(EXIT_SUCCESS);
		else
			printf("myshell: unknown command: %s\n", words[0]);

		nwords = 0;
	}

	return 0;
}
