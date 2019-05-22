// Template taken from hr-timer.c
// Revised by: Avneet Kaur
//             Feb 27, 2017
// Assignment 2 Task 3
 
 
#include <stdio.h>
#include <stdint.h>
#include <time.h> // for clock_gettime()
 
// returns difference between timeA_p anf timeB_p in nano seconds
unsigned long long timespecDiff(struct timespec *timeA_p, struct timespec *timeB_p)
{
  return ((timeA_p->tv_sec * 1000000000) + timeA_p->tv_nsec) -
           ((timeB_p->tv_sec * 1000000000) + timeB_p->tv_nsec); // change timeA_p->tv_sec to nanoseconds, add tv_nsec
                                                                // change timeB_p->tv_sec to nanoseconds, add tv_nsec
                                                                // take a difference of the two
}
 
 
 
int main()
{
struct timespec start; //create a timespec instance called start
struct timespec stop; // create a timespace instance called stop
unsigned long long result; //64 bit integer
 const int numberOfIterations = 10000;
unsigned long long averageResult=0;
int i=0;
 FILE *fp = fopen("systemcall.csv","w+");
fprintf(fp, "Iteration,Time Taken\n" );
for(i=0;i<numberOfIterations;i++){
 
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start); // retrieve time of Thread-specific CPU-time clock.
    pid_t temp_ = getpid();
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop); // retrieve time of Thread-specific CPU-time clock.
    unsigned long long temp = timespecDiff(&stop,&start); // get difference stop-start in nano seconds
	averageResult+= temp;
	
	fprintf(fp, "%d,%llu\n",i,temp);
 
}
 
 
result = averageResult/numberOfIterations;
fclose(fp);
printf("Minimal System Call Time: %llu ns\n",result); // print stop-start
//printf("CLOCK_THREAD_CPUTIME_ID Measured: %llu\n",result); // print stop-start
 
 
}