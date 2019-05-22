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

char *historyOfCommands[10]= { NULL }; // to store history (last 10 commands)
int updatedPositions[2] = {0,0}; // updatedPositions[0]=position of first element; 
								 // updatedPositions[1]=position of last element added;


// **********************************************************************
// Adds nextcommand to the circular array list containing 10 elements
// updates position of head and tail
// **********************************************************************
void addHistory(char **nextcommand){//char *list[10],char *nextcommand, int *positions){
	
	if(nextcommand==NULL || updatedPositions==NULL) {
		perror("Exiting");
		return;
	}
	else if( updatedPositions[0]<0 || updatedPositions[0]>9 || updatedPositions[1]<0 || updatedPositions[1]>9) {
		perror("Invalid positions");
		return;
	}

	char **next= malloc(sizeof(nextcommand));
	int head = updatedPositions[0]; //position of first element
	int tail = updatedPositions[1]; //position of next element to be added

	if (next==NULL){
		perror("Could not allocate memory");
		return;
	}
	//else
	strcpy(next,nextcommand);
	
	if(historyOfCommands[head]==NULL) {//empty historyOfCommands
		historyOfCommands[head]=next;
	}
	else {
		tail = (tail+1)%10; //update tail
		if(historyOfCommands[tail]!=NULL && tail==head) { //full historyOfCommands
			char *temp = historyOfCommands[tail];
			historyOfCommands[tail] = next;
			
			//tail = (tail+1)%10;
			head = (head+1)%10;

			free(temp);
		}
		else if(historyOfCommands[tail]==NULL){ 
			historyOfCommands[tail]=next;
			//printf("Added %s\n",historyOfCommands[tail] );
			//tail = (tail+1)%10;
		}
	}

	updatedPositions[0] = head;
	updatedPositions[1] = tail;
	//printf("Added new command: %s \n",next);

	return;
	
}


// ***********************************************
// View the last 10 commands entered in my_shell
// ***********************************************
void viewMyHistory(){ //char **historyOfCommands, int *positions){
	if(historyOfCommands==NULL) {
		printf("No History to display 1\n");
		return;
	}
	else if(updatedPositions == NULL){
		perror("Position array is empty");
		return;
	}
	else if( updatedPositions[0]<0 || updatedPositions[0]>9 || updatedPositions[1]<0 || updatedPositions[1]>9) {
		perror("Invalid positions");
		return;
	}
	int head=updatedPositions[0];
	int tail=updatedPositions[1];
	int i=head;
	int j=1;
	//printf("head is: %d, tail is: %d\n",head,tail );

	if(historyOfCommands[head]==NULL){
		printf("No History to display 2\n");
		return;
	}
	
	//printf("History of commands entered in my_shell: \n");
	do {
		printf("%d\t%s",j++,historyOfCommands[i]);
		i=(i+1)%10;

	}while(i!=tail && (historyOfCommands[i]!=NULL));

	//printf(" End of History\n");
	return;

}

// ********************************************************************
// Parse input & divide into tokens using space/tab/newline delimiter
// ********************************************************************
void tokenizeInput(char *command[], char *args[]){
	//char *command[1024];//command to be entered
	//char *args[50]; //array of tokens
	char **arguments = args; // array of arguments
	char *next_token = malloc(100); //temp token
	char *delimiters = " \t\n"; //  delimiters: space, tab, newline
	//parse and tokenize input
	arguments = args;
	next_token = strtok(command,delimiters);
	while (next_token!= NULL){
		*arguments++ = next_token;
		next_token = strtok(NULL,delimiters);
	}
	*arguments = NULL; //to get a null terminated list
	//printf("3.5tokenize\n");
	return;
}


// **********************************************************************
// Execute entered internal command using system call or ext command 
// using exec. if isChild=0, fork before exec
// **********************************************************************
void executeCommand(char *args[], int isChild){

	if(args==NULL || args[0]==NULL){
		perror("Empty command list");
		return;
	}
	//printf("1\n");
	// >> INTERNAL COMMANDS <<

	// exit
	if(strcmp(args[0],"exit")==0) {
		//printf("read exit\n");
		exit(1);
	}

	// cd
	else if(strcmp(args[0],"cd") == 0) {
		if(strcmp(args[1],"")!=0) {
			chdir(args[1]);
			//printf("%s ",myprompt);
			//continue;
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
	else if (strcmp(args[0],"history") == 0) {
		viewMyHistory();
	}


	// >> EXTERNAL COMMANDS <programs> <<
	else {
		//printf("1.5\n");
		if(isChild) {
			//printf("1.75\n");
			
			execvp(args[0],args);
			perror("exec failed");
		}
		else {

			pid_t pid = fork();
			if (pid <0) {
				perror("Fork failed!");
				exit(1);
			}
			else if(pid == 0) {
				//child process
				execvp(args[0],args);
				perror("exec failed");
			}
			//parent process
			wait(NULL);
			//printf("2\n");
		}
	}
	return;
}



int main() {

	char myprompt[] = ">>>>";
	char *command[1024];//command to be entered
	//char *args1[50],args2[50]; //array of tokens
	char *args[50];
	int numberOfPipes = 0;
	char *tempCommand = NULL;
	char *pipelinedCommands[50];

	signal(SIGINT, SIG_IGN); //ignore ctrl+C

	printf("%s ",myprompt);
	//printf("1\n");
	while(fgets(command, sizeof(command), stdin) != NULL) {
		//printf("2\n");
		numberOfPipes = 0;

		
		if(strcmp(command,"\n")==0){
			printf("%s ",myprompt);
			continue;
		}
		//printf("3\n");
		addHistory(command);
		//printf("4\n");

		// get a list of commands from pipes
		tempCommand = strtok(command,"|");
		//printf("5\n");
		while(tempCommand!=NULL){
			//printf("5.5\n");
			//*pipelinedCommands++ = tempCommand;
			//printf("temp command #%d: %s\n",numberOfPipes+1,tempCommand );
			pipelinedCommands[numberOfPipes++] = tempCommand;
			//printf("6\n");
			//printf("pipelinedCommands[%d]= %s\n",numberOfPipes-1,pipelinedCommands[numberOfPipes-1] );
			//numberOfPipes++;
			tempCommand = strtok(NULL,"|");
		}
		//*pipelinedCommands = NULL;
		pipelinedCommands[numberOfPipes]=NULL;
		//printf("7\n");
		numberOfPipes--; //get rid of 1 extra numberOfPipes
		//printf("number of pipes = %d\n", numberOfPipes);
		//printf("command 1 is %s\n",pipelinedCommands[0] );
		
		if(numberOfPipes==0 && pipelinedCommands[0]!=NULL){ //no pipes
			//printf("8\n");
			tokenizeInput(pipelinedCommands[0],args);
			executeCommand(args,0);
		}
		else if(pipelinedCommands[0]!=NULL){ // 1 or more pipes
			//printf("1\n");
			int counter = 0;
			int pipes[2*numberOfPipes]; //number of file desciptors for numberOfPipes #pipes = 2x
			int readFromfd = -1;
			int writeTofd = -1;
			int *current_pipe = pipes;
			int previousReadfd = -1;
			int previousWritefd = -1;
			int lastCommandSpawned = 0;

			// create pipes
			for(int i=0; i<numberOfPipes; i++){
				if(pipe(pipes+2*i) == -1) {
					perror("Pipe failed");
					exit(1);
				}
			}
			//printf("number of pipes = %d\n",numberOfPipes );
			//for(int i=0; i<2*numberOfPipes;i++){
			//	printf("pipes[%d]=%d\n",i,pipes[i] );
			//}
			while(pipelinedCommands[counter]!=NULL){
				//printf("2\n");
				//printf("counter= %d\n", counter );

				tokenizeInput(pipelinedCommands[counter],args);
				if(counter<numberOfPipes){
					//printf("updating fds when counter=%d\n",counter );
					readFromfd = current_pipe[0];
					writeTofd = current_pipe[1];
					
				}
				printf("previousreadfd: %d, readfd: %d, writefd: %d\n",previousReadfd,readFromfd, writeTofd );
				pid_t pid=fork();
				if(pid == -1){
					perror("fork failed");
					exit(1);
				}
				else if(pid==0){
					printf("executing command: %s\n", args[0]);
					if(counter==0) {
						//first command 
						//  > stdin for input
						//  > writeTofd for output
						// // {0:stdin,1:stdout,2:readfrom,4:writeto}
						printf("came to first if when counter=%d\n", counter);
						close(readFromfd);//STDOUT_FILENO); // {0:stdin,2:readfrom,4:writeto}
						//close(readFromfd); // {0:stdin,4:writeto}
						int returnvalue=dup2(writeTofd,STDOUT_FILENO); // {0:stdin,1:writeto,4:writeto}
						if(returnvalue==-1){
							perror("dup2 failed 1");
							exit(1);
						}

						//close(writeTofd); // {0:stdin,1:writeto}
						//printf("1readFromfd is now: %d\n",readFromfd );
						executeCommand(args,1); 

					}

					else if(counter==numberOfPipes) {
						//printf("previousreadfd: %d, readfd: %d, writefd: %d\n",previousReadfd,readFromfd, writeTofd );
						//last command
						// > readFromfd for input
						// > stdout for output
						// {0:stdin,1:stdout,2:readfrom,4:writeto}
						printf("came to the else if when counter=%d\n",counter);
						close(writeTofd);//STDIN_FILENO); // {1:stdout,2:readfrom,4:writeto}
						//close(writeTofd); // {1:stdout,2:readfrom}
						int returnvalue=dup2(previousReadfd,STDIN_FILENO); // {0:readfrom_prev,1:stdout,2:readfrom}
						if(returnvalue==-1){
							perror("dup2 failed 2");
							exit(1);
						}
						//close(previousReadfd);
						//printf("2readFromfd is now: %d\n",readFromfd );
						//printf("executing command...\n");
						//lastCommandSpawned = 1;
						//printf("   > lastCommandSpawned is set\n");
						//printf("  >> closing all pipes...\n");
						//for(current_pipe=pipes;current_pipe!=NULL;current_pipe++){
						//	close(*current_pipe);
						//}
						//for(int i=0;i<(2*numberOfPipes);i++){
						//	close(pipes[i]);
						//}
						//printf("  > closed all pipes\n");
						executeCommand(args,1); 
						
					}
					else {
						//niether first nor last
						// > readFromfd for input
						// > stdout for output
						// {0:stdin,1:stdout,2:readfrom,4:writeto}
						printf("came to else when counter=%d\n",counter);
						//close(writeTofd);//STDOUT_FILENO); // {0:stdin,2:readfrom,4:writeto}
						close(readFromfd); // {2:readfrom,4:writeto}
						//close(previousWritefd);
						int returnvalue1=dup2(writeTofd,STDOUT_FILENO); // {0:stdin,1:writeto,2:readfrom,4:writeto}
						int returnvalue2=dup2(previousReadfd,STDIN_FILENO); // {0:readfrom_prev,1:writeto,2:readfrom,4:writeto}
						int flag = 0;
						if(returnvalue1==-1){
							perror("dup2 failed for writeTofd");
							flag = 1;
							//exit(1);
						}
						else if(returnvalue2==-1){
							perror("dup2 failed for previousReadfd");
							flag = 1;
							//exit(1);
						}
						if(flag==1){
							exit(1);
						}
						//close(writeTofd);
						//close(previousReadfd);
						//printf("3readFromfd is now: %d\n",readFromfd );
						executeCommand(args,1); 

					}
				}
				counter++;
				current_pipe+=2;
				previousReadfd = readFromfd;
				previousWritefd = writeTofd;
				//printf("After a run: readFromfd=%d, previousReadfd=%d\n",readFromfd,previousReadfd );
				//printf("readFromfd for counter %d: %d\n",counter,readFromfd );

			}
			printf("Counter=%d\n", counter);
			printf("number of pipes = %d\n",numberOfPipes );
			if(counter==numberOfPipes+1){//lastCommandSpawned==1) { //last command
				printf("  >> closing all pipes...\n");
				//for(current_pipe=pipes;current_pipe!=NULL;current_pipe++){
				//	close(*current_pipe);
				//}
				for(int i=0;i<(2*numberOfPipes);i++){
					close(pipes[i]);
				}
				printf("  > closed all pipes\n");
			}

			// wait for all child processes
			printf("  > Waiting for all child processes\n");
			for(int i=0;i<=numberOfPipes;i++){
				//int status;
				wait(NULL);
			}
			// close any open file descriptors
			
			//printf("lastCommandSpawned= %d\n",lastCommandSpawned );
			

			
			printf("   > parent finished\n");
		}
		printf("%s ",myprompt);

	}
	
	//free(next_token);
	return 1;
}
