#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>


int main() {
	int n;
	const int BUFFSIZE = 4096;
	char buf[BUFFSIZE];
	char cwd[PATH_MAX];
	char *homedir;
	pid_t pid;	

	// Get home directory
	homedir = getenv("HOME");
	
	// Get current working directory
	if (getcwd(cwd, sizeof(cwd)) == NULL) {
		perror("getcwd error");
		return EXIT_FAILURE;
	}

	setbuf(stdout, NULL); // Disable buffering on stdout

	printf("Welcome to the 1730 shell.\n");
	
	while(1) {
		char dir[100] = "~"; 
		char *dirStart;

		dirStart = strstr(cwd, homedir);

		if (dirStart != NULL) {
			
			strcat(dir, dirStart += strlen(homedir));		
		} else {
			strcpy(dir, cwd);
		}

		printf("1730sh:%s$ ", dir);	
	
		// Getting user input from stdin
		n = read(STDIN_FILENO, buf, BUFFSIZE);
		
		// Convert buf into a null terminated string
		char str[n-1];
		strncpy(str, buf, n-1);
		str[n-1] = '\0';		

		// Convert string into array of args
		char *arg_array[100];
		int i = 0;
		int i2 = 0;
		char *p = strtok(str, " ");
		char *cmd;

		while (p != NULL) {
			if (i == 0) {
				cmd = p;
			}
			arg_array[i++] = p;
			p = strtok(NULL, " ");
		}
		arg_array[i] = NULL;

		// Check for exit command
		if (strcmp(cmd, "exit") == 0) {
			break;
		}
		if (strcmp(cmd, "cd") == 0) {
			chdir(arg_array[1]);
	 		getcwd(cwd, sizeof(cwd));
			continue;				
		}
			 
		// Fork
		switch (pid = fork()) {
			case -1:
				perror("child process creation failed");
				break;
			case 0: // child process				
				
				while (arg_array[i2] != NULL) {
					if (strcmp(arg_array[i2],"<")==0) {
						arg_array[i2] = NULL;
						int fdi = open(arg_array[i2+1], O_RDONLY);
						dup2(fdi, STDIN_FILENO);
					}

					if (strcmp(arg_array[i2],">")==0) {
						arg_array[i2] = NULL;
						int fdi = open(arg_array[i2+1], O_WRONLY|O_CREAT);
						dup2(fdi, STDOUT_FILENO);
					} 

					if (strcmp(arg_array[i2],">>")==0) {
						arg_array[i2] = NULL;
						int fdi = open(arg_array[i2+1], O_APPEND|O_CREAT);
						dup2(fdi, STDOUT_FILENO);
					}

					i2++;
				}
				
				if (execvp(cmd, arg_array) == -1) {
					perror("execvp error");
					exit(EXIT_FAILURE);
				}

				exit(EXIT_SUCCESS);
				break; 
			default: // parent process
				wait(NULL);
				break;
		}
					
	}
	
	return 0;
}
