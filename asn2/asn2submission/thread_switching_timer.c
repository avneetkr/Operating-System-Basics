/*****************************************************************************
 Excerpt from "Linux Programmer's Guide - Chapter 6"
 (C)opyright 1994-1995, Scott Burkett

 Modified for CMPT-300, to include 2 way communication
 ***************************************************************************** 
 MODULE: pipe.c
 *****************************************************************************/
 // Template taken from hr-timer.c and thread-example.c
// Revised by: Avneet Kaur
//             Mar 2, 2017
// Assignment 2 Task 5
#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>

//volatile unsigned long long c = 0;

// Thread variables
pthread_t t1, t2;
pthread_mutex_t lock;

int sharedVariable = 0;
const int iterations = 10000;
int j = 0;
// Timer variables //
struct timespec start; //create a timespec instance called start
struct timespec stop; // create a timespace instance called stop

unsigned long long averageResult=0;

FILE *fp;

unsigned long long timespecDiff(struct timespec *timeA_p, struct timespec *timeB_p)
{
  return ((timeA_p->tv_sec * 1000000000) + timeA_p->tv_nsec) -
           ((timeB_p->tv_sec * 1000000000) + timeB_p->tv_nsec);
}


void* thread1Add(){
        while(1){
            if(!pthread_mutex_lock(&lock)){

                clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);
                unsigned long long temp = timespecDiff(&stop,&start); // get difference stop-start in nano seconds
                averageResult+= temp;
    
                fprintf(fp, "%d,%llu\n",j++,temp);

                if(sharedVariable == 1){
                  sharedVariable = 0;
                 }

                pthread_mutex_unlock(&lock);
                break;
            }
        }
    return;

}

void* thread2Add(){
        while(1){
            if(!pthread_mutex_lock(&lock)){
                if(sharedVariable == 0){
                    sharedVariable = 1;
                }
                clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start); // retrieve time of Thread-specific CPU-time clock.
                pthread_mutex_unlock(&lock);
                break;
            }
        }
    
     return;
}

int main()
{

    // affinity variables
    
    cpu_set_t my_set;
    int returnvalue;
    int i=0;
    CPU_ZERO(&my_set);
    CPU_SET(1, &my_set);
    returnvalue = sched_setaffinity(0, sizeof(my_set), &my_set);
    if (returnvalue == -1) {
        perror("sched_setaffinity failed");
    }
    
    fp = fopen("threadswitching.csv","w+");
    fprintf(fp, "Iteration,Time Taken\n" );
    //***********************************************************************************
  
    int rt1, rt2;
    unsigned long long result; //64 bit integer
      	
    while(i<iterations){
        // initialise mutex lock
        if (pthread_mutex_init(&lock, NULL) != 0)
        {
             perror("mutex init failed");
            return 1;
        }

        sharedVariable = 0;
        /* Create 2 threads */
        if( (rt1=pthread_create( &t1, NULL, &thread1Add, NULL)) )
            printf("Thread creation failed: %d\n", rt1);
        if( (rt2=pthread_create( &t2, NULL, &thread2Add, NULL)) )
            printf("Thread creation failed: %d\n", rt2);

        /* Wait for all threads to finish */
        pthread_join( t1, NULL);
        pthread_join( t2, NULL);
           
        pthread_mutex_destroy(&lock);

        i++;
    }

    result = averageResult/iterations;
    fclose(fp);
    printf("Threaded Time(ns): %llu\n",result);

    return 0;

    }
