/*
* Template taken from "Hints for Assignment 1"
* Revised by: Avneet Kaur, Date: February 1, 2017
*
*
* Objective:
* Build a basic shell that supports 
* > internal commands: exit, pwd, history and cd
* > external commands in the following format:
* 		<program name> <arg1> <arg2> ... <argN>		
*
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

int main() {
	char myprompt[] = ">>>>";//"Please enter your command: \n";
	char *command[1024];//command to be entered
	char *args[50]; //arguments: head tokenized command
	char **arguments = args; // array of arguments
	char *next_token = malloc(100); //temp token
	char* delimiters = " \t\n"; //delimiters: space, tab, newline
	

	signal(SIGINT, SIG_IGN); //ignore ctrl+C
	
	printf("%s ",myprompt);
	while(fgets(command, sizeof(command), stdin) != NULL) {
		
		

		//parse and tokenize input
		arguments = args;
		next_token = strtok(command,delimiters);
		while (next_token!= NULL){
			*arguments++ = next_token;
			//i++;
			//printf("argument %d %s\n",i,next_token);
			next_token = strtok(NULL,delimiters);
		}
		*arguments = NULL;
		// >> INTERNAL COMMANDS <<

		// exit
		if(strcmp(args[0],"exit")==0) {
			exit(1);
		}

		// cd
		else if(strcmp(args[0],"cd") == 0) {
			if(strcmp(args[1],"")!=0) {
				chdir(args[1]);
				printf("%s ",myprompt);
				continue;
			}
			else {
				perror("Could not change directory");
			}
		}

		// pwd
		else if (strcmp(args[0],"pwd") == 0) {
			char* pwd_output[1024];
			if(getcwd(pwd_output,sizeof(pwd_output))!=NULL) {
				printf("%s\n",pwd_output);
			}
			else {
				perror("Could not read current working directory");
			}
		}

		// history
		// >>>>add here<<<<


		// >> EXTERNAL COMMANDS <programs> <<

		//for(arguments=args; *arguments!=NULL;arguments++)
		//	printf("%s\n", *arguments);
		else {

			pid_t pid = fork();
			//printf("The command entered is: %s",command);
			if (pid <0) {
				perror("Fork failed!");
				exit(1);
			}
			else if(pid == 0) {
				//child process
				execvp(args[0],args);
			}
			//parent process
			wait(NULL);
		}
		printf("%s ",myprompt);

	}
	
	free(next_token);
	return 1;
}