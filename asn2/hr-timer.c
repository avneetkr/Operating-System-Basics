// Assignment 2 Task 1

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

/* As specified in <time.h>:

struct timespec {
        time_t   tv_sec;        // seconds 
        long     tv_nsec;       // nanoseconds
};

	
use of different clock depends on what you want to measure. 
CLOCK_MONOTONIC should be used if you want to measure total elapsed time, 
including time spent blocked waiting for IO, but it will also include slowdowns caused by other 
processes getting scheduled while your program is trying to run. 
CLOCK_PROCESS_CPUTIME_ID will only count actual clock cycles spent executing on your process's 
behalf (in either userspace or kernelspace, I believe), 
but not any time spent blocked/sleeping.
*/

int main()
{
struct timespec start; //create a timespec instance called start
struct timespec stop; // create a timespace instance called stop
unsigned long long result; //64 bit integer

clock_gettime(CLOCK_REALTIME, &start); // retrieve time of system-wide realtime clock 
sleep(1);  // sleep for 1 second
clock_gettime(CLOCK_REALTIME, &stop); // retrieve time of system-wide realtime clock

result=timespecDiff(&stop,&start); // get difference stop-start in nano seconds

printf("CLOCK_REALTIME Measured: %llu\n",result); // print stop-start

clock_gettime(CLOCK_MONOTONIC, &start); // retrieve time of CLOCK_MONOTONIC( i.e. Clock that cannot be set and 
										// represents monotonic time since some unspecified starting point.) 

sleep(1); 
clock_gettime(CLOCK_MONOTONIC, &stop); // retrieve time of CLOCK_MONOTONIC

result=timespecDiff(&stop,&start); // get difference stop-start in nano seconds

printf("CLOCK_MONOTONIC Measured: %llu\n",result); // print stop-start

clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start); // retrieve time of High-resolution per-process timer from the CPU.
sleep(1); // sleep for 1 second
clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop); // retrieve time of High-resolution per-process timer from the CPU.

result=timespecDiff(&stop,&start); // get difference stop-start in nano seconds

printf("CLOCK_PROCESS_CPUTIME_ID Measured: %llu\n",result); // print stop-start

clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start); // retrieve time of Thread-specific CPU-time clock.
sleep(1); // sleep for 1 second
clock_gettime(CLOCK_THREAD_CPUTIME_ID, &stop); // retrieve time of Thread-specific CPU-time clock.

result=timespecDiff(&stop,&start); // get difference stop-start in nano seconds

printf("CLOCK_THREAD_CPUTIME_ID Measured: %llu\n",result); // print stop-start


}