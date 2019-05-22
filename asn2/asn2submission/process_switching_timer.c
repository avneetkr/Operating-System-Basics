/*****************************************************************************
 Excerpt from "Linux Programmer's Guide - Chapter 6"
 (C)opyright 1994-1995, Scott Burkett

 Modified for CMPT-300, to include 2 way communication
 ***************************************************************************** 
 MODULE: pipe.c
 *****************************************************************************/
 // Template taken from hr-timer.c
// Revised by: Avneet Kaur
//             Mar 2, 2017
// Assignment 2 Task 4
 

#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <time.h> // for clock_gettime()

// returns difference between timeA_p anf timeB_p in nano seconds
unsigned long long timespecDiff(struct timespec *timeA_p, struct timespec *timeB_p)
{
  return ((timeA_p->tv_sec * 1000000000) + timeA_p->tv_nsec) -
           ((timeB_p->tv_sec * 1000000000) + timeB_p->tv_nsec); // change timeA_p->tv_sec to nanoseconds, add tv_nsec
           														// change timeB_p->tv_sec to nanoseconds, add tv_nsec
           														// take a difference of the two
}

int main(void){

	// affinity variables
	cpu_set_t my_set;
  	int returnvalue;
  	
  	CPU_ZERO(&my_set);
	CPU_SET(1, &my_set);
	returnvalue = sched_setaffinity(0, sizeof(my_set), &my_set);
	if (returnvalue == -1) {
		perror("sched_setaffinity failed");
	}
    FILE *fp = fopen("processswitching.csv","w+");
	fprintf(fp, "Iteration,Time Taken\n" );

	//***********************************************************************************
	// Timer variables //
	struct timespec start; //create a timespec instance called start
	struct timespec stop; // create a timespace instance called stop
	unsigned long long result; //64 bit integer

	unsigned long long averageResult=0;
	int i=0;
	
	//***********************************************************************************
	// pipe & process variables
	int fdP[2]; //Pipe child uses to contact parent
	int fdC[2]; //Pipe parent uses to contact child

	int nbytes;
   	pid_t   childpid;
    	unsigned char   returnString = '~';//"Thank you for telling me!";
   	unsigned char   readbuffer;
	unsigned char inputBuffer = '*';

    	const int numberOfIterations = 10000;
    //***********************************************************************************    


	
		
		//***********************************************************************************
		pipe(fdP); // Child >> Parent
		pipe(fdC); // Parent >> Child 
		//printf("Please send a character\n");
		//printf("1\n");
	    if((childpid = fork()) == -1)
	    {
	            perror("fork");
	            exit(1);
	    }

	    if(childpid == 0){
	    	CPU_ZERO(&my_set);
    		CPU_SET(1, &my_set);
    		returnvalue = sched_setaffinity(0, sizeof(my_set), &my_set);
			if (returnvalue == -1) {
				perror("sched_setaffinity failed");
			}
	    	close(fdC[0]); // The Child closes the output side of its pipe
			close(fdP[1]); // The Child also closes the parents input side

			for(i=0;i<numberOfIterations;i++){

				bzero((char *) &readbuffer, sizeof(readbuffer)); //Clear buffer
				nbytes = read(fdP[0], &readbuffer, sizeof(readbuffer)); // Child blocks on read
				if((write(fdC[1], &returnString, sizeof(returnString)))==-1){ // Send an ACK
					perror("Write failed");
				}
			}

			close(fdC[1]);
			close(fdP[0]);
	        exit(0);
	    }

	    else {

	    	close(fdC[1]);
			close(fdP[0]);

			for(i=0;i<numberOfIterations;i++){

				bzero((char *) &inputBuffer,sizeof(inputBuffer) );//sizeof(inputBuffer));

				clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start); // retrieve time of Thread-specific CPU-time clock.

				write(fdP[1], &inputBuffer, sizeof(inputBuffer));
				nbytes = read(fdC[0], &readbuffer,sizeof(readbuffer));

				clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop); // retrieve time of Thread-specific CPU-time clock.

				unsigned long long temp = timespecDiff(&stop,&start); // get difference stop-start in nano seconds
				averageResult+= temp;
	
				fprintf(fp, "%d,%llu\n",i,temp);
			}

			wait(NULL);
			close(fdC[0]);
			close(fdP[1]);
	   }
	   //***********************************************************************************

	result = averageResult/numberOfIterations;
	fclose(fp);
	printf("Minimal Process Switching Time: %llu ns\n",result); // print stop-start
    return(0);
}

																																													
