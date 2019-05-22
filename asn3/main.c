/*
 * Avneet Kaur 
 * avneetk@sfu.ca
 * 301209987
 *
 * main.c
 */

#include "main.h"
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
void *fnC()
{
    int i;
    for(i=0;i<1000000;i++)
    {   
        c++;
    }   
}


pthread_mutex_t count_mutex;
pthread_spinlock_t count_spinlock;
my_spinlock_t my_spinlock;
my_queuelock_t my_queuelock;
my_mutex_t count_my_mutex;


void *pthreadMutexTest()
{
	
    int i;
	int j;
	int k;
	
	int localCount = 0;
	
    for(i=0;i<numItterations;i++)
    {
		
		for(j=0;j<workOutsideCS;j++)/*How much work is done outside the CS*/
		{
			localCount++;
		}
		
		pthread_mutex_lock(&count_mutex);
		for(k=0;k<workInsideCS;k++)/*How much work is done inside the CS*/
		{
			c++;
		}
		pthread_mutex_unlock(&count_mutex);    
	
    }   


}

//*******************************************************************
// pthread Spinlock test
//*******************************************************************
void *pthreadSpinlockTest(){
	int i,j,k;
	int localCount = 0;

    for(i=0;i<numItterations;i++)
    {
		
		for(j=0;j<workOutsideCS;j++)/*How much work is done outside the CS*/
		{
			localCount++;
		}
		
		pthread_spin_lock(&count_spinlock);
		for(k=0;k<workInsideCS;k++)/*How much work is done inside the CS*/
		{
			c++;
		}
		pthread_spin_unlock(&count_spinlock);    
	
    }   
}
//*******************************************************************
// mySpinLockTAS  test
//*******************************************************************
void *mySpinlockTASTest(){
	int i,j,k;
	int localCount = 0;
	printf("spinlockTAS <<<\n");
    for(i=0;i<numItterations;i++)
    {
		
		for(j=0;j<workOutsideCS;j++)/*How much work is done outside the CS*/
		{
			localCount++;
		}
		//printf("blahblah thread\n");
		int ret1 = my_spinlock_lockTAS(&my_spinlock);
		for(k=0;k<workInsideCS;k++)/*How much work is done inside the CS*/
		{
			c++;
		}
		int ret2 = my_spinlock_unlock(&my_spinlock);    
	
    }   
}

//*******************************************************************
// mySpinLockTTAS test
//*******************************************************************
void *mySpinlockTTASTest(){
	int i,j,k;
	int localCount = 0;

    for(i=0;i<numItterations;i++)
    {
		
		for(j=0;j<workOutsideCS;j++)/*How much work is done outside the CS*/
		{
			localCount++;
		}
		
		my_spinlock_lockTTAS(&my_spinlock);
		for(k=0;k<workInsideCS;k++)/*How much work is done inside the CS*/
		{
			c++;
		}
		my_spinlock_unlock(&my_spinlock);    
	
    }   
}
//*******************************************************************
// myMutexTAS test
//*******************************************************************
void *myMutexTest(){
	int i,j,k;
	int localCount = 0;

    for(i=0;i<numItterations;i++)
    {
		
		for(j=0;j<workOutsideCS;j++)/*How much work is done outside the CS*/
		{
			localCount++;
		}
		
		my_mutex_lock(&count_my_mutex);
		for(k=0;k<workInsideCS;k++)/*How much work is done inside the CS*/
		{
			c++;
		}
		my_mutex_unlock(&count_my_mutex);    
	
    }   
}
//*******************************************************************
//  myQueueLock test
//*******************************************************************
void *myQueueLockTest(){
	int i,j,k;
	int localCount = 0;

    for(i=0;i<numItterations;i++)
    {
		
		for(j=0;j<workOutsideCS;j++)/*How much work is done outside the CS*/
		{
			localCount++;
		}
		
		my_queuelock_lock(&my_queuelock);
		for(k=0;k<workInsideCS;k++)/*How much work is done inside the CS*/
		{
			c++;
		}
		my_queuelock_unlock(&my_queuelock);    
	
    }   
}



//********************************************************************
//
int runTest(int testID)
{

/*You must create all data structures and threads for you experiments*/

if (testID == 0 || testID == 1 ) /*Pthread Mutex*/
{
	c=0;
	struct timespec start;
	struct timespec stop;
	unsigned long long result; //64 bit integer

	pthread_t *threads = (pthread_t* )malloc(sizeof(pthread_t)*numThreads);	
	int i;
	int rt;

	clock_gettime(CLOCK_MONOTONIC, &start);
	for(i=0;i<numThreads;i++)
	{
	
	 if( rt=(pthread_create( threads+i, NULL, &pthreadMutexTest, NULL)) )
	{
		printf("Thread creation failed: %d\n", rt);
		return -1;	
	}
	
	}
	
	for(i=0;i<numThreads;i++) //Wait for all threads to finish
	{
		 pthread_join(threads[i], NULL);
	}
	clock_gettime(CLOCK_MONOTONIC, &stop);

	printf("Threaded Run Pthread (Mutex) Total Count: %llu\n", c);
	result=timespecDiff(&stop,&start);
	printf("Pthread Mutex time(ms): %llu\n",result/1000000);

}

if(testID == 0 || testID == 2) /*Pthread Spinlock*/
{
/*Pthread Spinlock goes here*/
	int returnvalue = pthread_spin_init(&count_spinlock,PTHREAD_PROCESS_PRIVATE);
	c=0;
	struct timespec start;
	struct timespec stop;
	unsigned long long result; //64 bit integer

	pthread_t *threads = (pthread_t* )malloc(sizeof(pthread_t)*numThreads);	
	int i;
	int rt;

	clock_gettime(CLOCK_MONOTONIC, &start);
	for(i=0;i<numThreads;i++)
	{
	
	 if( rt=(pthread_create( threads+i, NULL, &pthreadSpinlockTest, NULL)) )
	{
		printf("Thread creation failed: %d\n", rt);
		return -1;	
	}
	
	}
	
	for(i=0;i<numThreads;i++) //Wait for all threads to finish
	{
		 pthread_join(threads[i], NULL);
	}
	clock_gettime(CLOCK_MONOTONIC, &stop);

	printf("Threaded Run Pthread (Spinlock) Total Count: %llu\n", c);
	result=timespecDiff(&stop,&start);
	printf("Pthread Spinlock time(ms): %llu\n",result/1000000);
}

if(testID == 0 || testID == 3) /*MySpinlockTAS*/
{
	int returnvalue = my_spinlock_init(&my_spinlock);
/* mySpinlock TAS goes here*/
	c=0;
	struct timespec start;
	struct timespec stop;
	unsigned long long result; //64 bit integer

	pthread_t *threads = (pthread_t* )malloc(sizeof(pthread_t)*numThreads);	
	int i;
	int rt;

	clock_gettime(CLOCK_MONOTONIC, &start);
	for(i=0;i<numThreads;i++)
	{
	
	 if( rt=(pthread_create( threads+i, NULL, &mySpinlockTASTest, NULL)) )
	{
		printf("Thread creation failed: %d\n", rt);
		return -1;	
	}
	
	}
	
	for(i=0;i<numThreads;i++) //Wait for all threads to finish
	{
		 pthread_join(threads[i], NULL);
	}
	clock_gettime(CLOCK_MONOTONIC, &stop);

	printf("Threaded Run mySpinlock TAS Test Total Count: %llu\n", c);
	result=timespecDiff(&stop,&start);
	printf("mySpinlock TAS time(ms): %llu\n",result/1000000);
}

if(testID == 0 || testID == 4) /*MySpinlockTTAS*/
{
	my_spinlock_init(&my_spinlock);
/* mySpinlock TTAS goes here*/
	c=0;
	struct timespec start;
	struct timespec stop;
	unsigned long long result; //64 bit integer

	pthread_t *threads = (pthread_t* )malloc(sizeof(pthread_t)*numThreads);	
	int i;
	int rt;

	clock_gettime(CLOCK_MONOTONIC, &start);
	for(i=0;i<numThreads;i++)
	{
	
	 if( rt=(pthread_create( threads+i, NULL, &mySpinlockTTASTest, NULL)) )
	{
		printf("Thread creation failed: %d\n", rt);
		return -1;	
	}
	
	}
	
	for(i=0;i<numThreads;i++) //Wait for all threads to finish
	{
		 pthread_join(threads[i], NULL);
	}
	clock_gettime(CLOCK_MONOTONIC, &stop);

	printf("Threaded Run mySpinlock TTAS Test Total Count: %llu\n", c);
	result=timespecDiff(&stop,&start);
	printf("mySpinlock TTAS time(ms): %llu\n",result/1000000);
}


if(testID == 0 || testID == 5) /*MyMutexTAS*/
{
	my_mutex_init(&count_my_mutex);
	c=0;
	struct timespec start;
	struct timespec stop;
	unsigned long long result; //64 bit integer

	pthread_t *threads = (pthread_t* )malloc(sizeof(pthread_t)*numThreads);	
	int i;
	int rt;

	clock_gettime(CLOCK_MONOTONIC, &start);
	for(i=0;i<numThreads;i++)
	{
	
	 if( rt=(pthread_create( threads+i, NULL, &myMutexTest, NULL)) )
	{
		printf("Thread creation failed: %d\n", rt);
		return -1;	
	}
	
	}
	
	for(i=0;i<numThreads;i++) //Wait for all threads to finish
	{
		 pthread_join(threads[i], NULL);
	}
	clock_gettime(CLOCK_MONOTONIC, &stop);

	printf("Threaded Run myMutex Test Total Count: %llu\n", c);
	result=timespecDiff(&stop,&start);
	printf("myMutex time(ms): %llu\n",result/1000000);
 
}
if(testID == 0 || testID == 6) /*MyQueueLock*/
{
	my_queuelock_init(&my_queuelock);
	c=0;
	struct timespec start;
	struct timespec stop;
	unsigned long long result; //64 bit integer

	pthread_t *threads = (pthread_t* )malloc(sizeof(pthread_t)*numThreads);	
	int i;
	int rt;

	clock_gettime(CLOCK_MONOTONIC, &start);
	for(i=0;i<numThreads;i++)
	{
	
	 if( rt=(pthread_create( threads+i, NULL, &myQueueLockTest, NULL)) )
	{
		printf("Thread creation failed: %d\n", rt);
		return -1;	
	}
	
	}
	
	for(i=0;i<numThreads;i++) //Wait for all threads to finish
	{
		 pthread_join(threads[i], NULL);
	}
	clock_gettime(CLOCK_MONOTONIC, &stop);

	printf("Threaded Run myQueueLock Test Total Count: %llu\n", c);
	result=timespecDiff(&stop,&start);
	printf("myQueueLock time(ms): %llu\n",result/1000000);
 
}
	return 0;
}

int testAndSetExample()
{
volatile long test = 0; //Test is set to 0
printf("Test before atomic OP:%d\n",test);
tas(&test);
printf("Test after atomic OP:%d\n",test);
}


// to process input from the command line and set the testing variables
int processInput(int argc, char *argv[])
{

/*testid: 0=all, 1=pthreadMutex, 2=pthreadSpinlock, 3=mySpinLockTAS, 4=mySpinLockTTAS, 5=myMutexTAS, 6=myQueueLock*/
	/*You must write how to parse input from the command line here, your software should default to the values given below if no input is given*/
	
	numThreads=4;
	numItterations=1000000;
	testID=0;
	workOutsideCS=0;
	workInsideCS=1;
	int i = 0;
	if(argc ==0 ||argv==NULL || argv[0]==NULL){
		perror("Empty command list");
		return -1;
	}
	
	
	//while(argv[i]!= NULL || argv[i] != EOF() ){ // >>> confirm EOF usage
	while( i<argc ) {
		if( (strcmp(argv[i],"-t")==0) && (argv[i+1]!=NULL)) {
			numThreads = atoi(argv[i+1]); // >> if invalid input, returns 0
			i +=2;
			//printf("read numThreads\n");
		}
		else if( (strcmp(argv[i],"-i") == 0) && (argv[i+1]!=NULL)){
			numItterations = atoi(argv[i+1]);
			i+=2;
			//printf("read numItterations\n");
		}
		else if((strcmp(argv[i],"-o") == 0) && (argv[i+1]!=NULL)){
			workOutsideCS = atoi(argv[i+1]);
			i+=2;
			//printf("read workOutsideCS\n");
		}
		else if((strcmp(argv[i],"-c") == 0) && (argv[i+1]!=NULL)){
			workInsideCS = atoi(argv[i+1]);
			i+=2;
			//printf("workInsideCSk\n");
		}
		else if((strcmp(argv[i],"-d") == 0) && (argv[i+1]!=NULL)){
			testID = atoi(argv[i+1]);
			i+=2;
			//printf("reAD testid\n");
		}
		else {
			i++;
		}
	}
	
	
	
	return 0;
}


int main(int argc, char *argv[])
{


	printf("Usage of: %s -t #threads -i #Itterations -o #OperationsOutsideCS -c #OperationsInsideCS -d testid\n", argv[0]);
	printf("testid: 0=all, 1=pthreadMutex, 2=pthreadSpinlock, 3=mySpinLockTAS, 4=mySpinLockTTAS, 5=myMutexTAS, 6=myQueueLock, \n");	
	
	//testAndSetExample(); //Uncomment this line to see how to use TAS
	//for(int i=0;i<argc;i++){
	//	printf("argv[%d]: %s\n",i,argv[i]);
	//}
	processInput(argc,argv);
	//printf("done processing input\n");
	runTest(testID);
	return 0;

}
