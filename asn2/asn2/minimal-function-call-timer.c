// Template taken from hr-timer.c
// Revised by: Avneet Kaur
//			   Feb 27, 2017
// Assignment 2 Task 2


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

void testfunc(){
	return;
}

int main()
{


struct timespec start; //create a timespec instance called start
struct timespec stop; // create a timespace instance called stop
unsigned long long result; //64 bit integer


unsigned long long averageResult=0;
int i=0;

FILE *fp = fopen("functioncall.csv","w+");
fprintf(fp, "Iteration,Time Taken\n" );


for(i=0;i<10;i++){

	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start); // retrieve time of Thread-specific CPU-time clock.
	testfunc();
	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &stop); // retrieve time of Thread-specific CPU-time clock.
	unsigned long long temp = timespecDiff(&stop,&start); // get difference stop-start in nano seconds
	averageResult+= temp;
	
	fprintf(fp, "%lu\n",temp);

}


result = averageResult/10;

printf("Minimal Function Call Time: %llu ns\n",result); // print stop-start
//printf("CLOCK_THREAD_CPUTIME_ID Measured: %llu\n",result); // print stop-start


}