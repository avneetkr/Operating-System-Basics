#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <time.h>

volatile unsigned long long c = 0;


unsigned long long timespecDiff(struct timespec *timeA_p, struct timespec *timeB_p)
{
  return ((timeA_p->tv_sec * 1000000000) + timeA_p->tv_nsec) -
           ((timeB_p->tv_sec * 1000000000) + timeB_p->tv_nsec);
}


void *fnC()
{
    int i;
    for(i=0;i<100000000;i++)
    {   
        c++; 
    }   
}


int main()
{


struct timespec start;
struct timespec stop;
unsigned long long int result; //64 bit integer


clock_gettime(CLOCK_MONOTONIC, &start);
	c=0;
    int i;

	fnC();
	fnC();
	fnC();
	fnC();
	
	
	clock_gettime(CLOCK_MONOTONIC, &stop);
result=timespecDiff(&stop,&start);


result=timespecDiff(&stop,&start);

printf("Single Thread Run total count %d\n", c);

printf("1 thread time: %llu\n",result/1000000);



    int rt1, rt2, rt3, rt4;
    pthread_t t1, t2, t3 ,t4; 
	c=0;

clock_gettime(CLOCK_MONOTONIC, &start);
    /* Create 4 threads */
    if( (rt1=pthread_create( &t1, NULL, &fnC, NULL)) )
        printf("Thread creation failed: %d\n", rt1);
 if( (rt2=pthread_create( &t2, NULL, &fnC, NULL)) )
        printf("Thread creation failed: %d\n", rt2);
 if( (rt3=pthread_create( &t3, NULL, &fnC, NULL)) )
        printf("Thread creation failed: %d\n", rt3);
if( (rt4=pthread_create( &t4, NULL, &fnC, NULL)) )
        printf("Thread creation failed: %d\n", rt4);
	
	
	
	

    /* Wait for all threads to finish */
    pthread_join( t1, NULL);
    pthread_join( t2, NULL);
    pthread_join( t3, NULL);
    pthread_join( t4, NULL);

clock_gettime(CLOCK_MONOTONIC, &stop);


result=timespecDiff(&stop,&start);
    printf("Threaded Run total count %d", c);
    printf ("\n");
printf("Threaded Time(ms): %llu\n",result/1000000);
    c=0;
printf ("\n");



	return 0;

}
