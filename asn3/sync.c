
/*
 * Avneet Kaur 
 * avneetk@sfu.ca
 * 301209987
 *
 * sync.c
 *
 *
 */

#define _REENTRANT
#include <pthread.h>
#include "sync.h"
#include <stdlib.h>
#include <time.h> //time()
#include <sys/types.h>
#include <sys/syscall.h> //for syscall __NR_gettid
#include <unistd.h> // for usleep
/*
 * Spinlock routines
 */

int my_spinlock_init(my_spinlock_t *lock)
{
        lock->lock_value = 0;
        lock->lockptr = &(lock->lock_value);
        lock->recursive_counter = 0;
        lock->ownerID = 0;
        return 0;
}


int my_spinlock_destroy(my_spinlock_t *lock)
{
        // ?? nothing to destroy
	return 0;
}


int my_spinlock_unlock(my_spinlock_t *lock)
{
    if(lock->recursive_counter == 0 || lock->recursive_counter <0){
		//(lock->lock_value) = 0;
		__sync_lock_test_and_set(lock->lockptr, 0);
		 //pid_t x = syscall(__NR_gettid);
		  //printf("thread %d released the lock\n",x );
	}
	else{
		(lock->recursive_counter)--;
		//printf("why are you here???\n");
	}
	return 0;
}


int my_spinlock_lockTAS(my_spinlock_t *lock)
{
 
    pid_t x = syscall(__NR_gettid);
	

	if(lock->ownerID == x){ //
	//current owner of lock is trying to get the lock again
		(lock->recursive_counter)++;
		//printf("thread %d recursive_counter =%d\n",x,lock->recursive_counter );
	}
	
	else{	// regular wait for acquiring lock
		while(tas((lock->lockptr)) == 1){ // if  tas returns 1, => someone is using that lock
                //spin
        }
        lock->ownerID = x;
        lock->recursive_counter = 0;
        //on acquiring the lock, set ownerID to current thread ID
        //printf("thread %d got the lock\n",x );
        
        usleep(5);
	}
	
	return 0;
}


int my_spinlock_lockTTAS(my_spinlock_t *lock)
{
	pid_t x = syscall(__NR_gettid);

	if(lock->ownerID == x){ 
	(lock->recursive_counter)++;
	}
		// regular wait for acquiring lock
	else{
	    while(1){ // lurking stage
	        while(tas((lock->lockptr)) == 1){
	            if(tas((lock->lockptr))==0){// pouncing stage
	                //on acquiring the lock, set ownerID to current thread ID
        			lock->ownerID = x;
        			lock->recursive_counter = 0;
	                return 0;
	            }
	        }
	            
	    }
	}
}

int my_spinlock_trylock(my_spinlock_t *lock)
{
        // spin_trylock() does not spin but returns non-zero if it acquires the 
        // spinlock on the first try or 0 if not. This function can be used in all 
        // contexts like spin_lock: you must have disabled the contexts that might 
        // interrupt you and acquire the spin lock->

        //return(!tas(lock));
        // returns 0 if it's already acquired
        	//return(!tas(lock));
	pid_t x = syscall(__NR_gettid);

	if(lock->ownerID == x)
	{// lock successfully acquired on first try
		(lock->recursive_counter)++;
		return 1;
	}
	else{
		if(tas((lock->lockptr))){
			return 0;
		}
		else{// lock successfully acquired on first try
			lock->ownerID = x;
        	lock->recursive_counter = 0;
        	return 1;
		}
		
	}
	

}



/*
 * Mutex routines
 */

int my_mutex_init(my_mutex_t *lock)
{
	lock->lock_value = 0;
    lock->lockptr = &(lock->lock_value);

    lock->delay = MIN_DELAY;

    srand((unsigned int)time(NULL));
    lock->recursive_counter = 0;
    lock->ownerID = 0;
    
    return 0;
}

int my_mutex_destroy(my_mutex_t *lock)
{

	return 0;
}

int my_mutex_unlock(my_mutex_t *lock)
{
   
    if(lock->recursive_counter == 0){
		(lock->lock_value) = 0;
	}
	else{
		(lock->recursive_counter)--;
	}
	return 0;
}

int my_mutex_lock(my_mutex_t *lock)
{
	

	pid_t x = syscall(__NR_gettid);

	if(lock->ownerID != x){ //
		//mutually exclusive and exponential backoff
		lock->delay = MIN_DELAY;
		while(1){
			while(tas(lock->lockptr)==1){
				if(tas(lock->lockptr)==0){
					//on acquiring the lock, set ownerID to current thread ID
        			lock->ownerID = x;
        			lock->recursive_counter = 0;
					return 0;
				}
				// confirm random() syntax
				usleep(rand() % (lock->delay));
				if(lock->delay < MAX_DELAY){
					lock->delay = 2*(lock->delay);
				}

			}
		}

	}

	else{//current owner of lock is trying to get the lock again
		
		(lock->recursive_counter)++;

	}
}

int my_mutex_trylock(my_mutex_t *lock)
{
	//return(!tas(lock));
	pid_t x = syscall(__NR_gettid);

	if(lock->ownerID != x){
		if(tas(lock->lockptr)){
			return 0;
		}
		else{// lock successfully acquired on first try
			lock->ownerID = x;
        	lock->recursive_counter = 0;
        	return 1;
		}
		
	}
	else{// lock successfully acquired on first try
		(lock->recursive_counter)++;
		return 1;
	}

}



/*
 * Queue Lock
 */

int my_queuelock_init(my_queuelock_t *lock)
{
	//lock_value = 0;
    //lock = &lock_value;

 	lock->current_position = 0;//now_serving
    lock->next_available_position = 0;
    //head = NULL;
    //tail = NULL;
    //size = 0;

    return 0;
}
//int* p = malloc(sizeof(*p));
int my_queuelock_destroy(my_queuelock_t *lock)
{	
	/*if(size!=0){
		node *p = head;
		node *temp =  head;
		while(p!=NULL){
			temp = p;
			p = p->next;
			free(temp);
			size--;
		}
	}
	head = NULL;
	tail = NULL;
	return 0;*/
	return 0;
}

int my_queuelock_unlock(my_queuelock_t *lock)
{
	/*if((lock->head)!=NULL){
		*lock = 0;
		if ((lock->head)->next)!=NULL)
		{
			

		}
	}*/

	(lock->current_position)++;


}

int my_queuelock_lock(my_queuelock_t *lock)
{
	/*int my_position = fai(lock->next_available_position);
	node* p = malloc(sizeof(*p));
	p->position = my_position;
	p->next = NULL;

	// add to queue
	if(size!=0){
	(lock->tail)-> next = p;
	lock->tail = (lock->tail)->next;
	(lock->size)++;
	}
	else{
		lock->head = p;
		lock->tail = p;
		(lock->size)++;
	}
	while(lock->tail != my_position){
		//spin
	}
	// update current_position after the lock is acquired
	lock->current_position = my_position;
	return 0;*/


	pid_t x = syscall(__NR_gettid);

	if(lock->ownerID != x){
		int my_position = fai(&(lock->next_available_position));
		while((lock->current_position)!=my_position){
			//spin
		}
		lock->ownerID = x;
        lock->recursive_counter = 0;
	}
	else{
		(lock->recursive_counter)++;
	}

	return 0;

}

int my_queuelock_trylock(my_queuelock_t *lock)
{
	pid_t x = syscall(__NR_gettid);

	if(lock->ownerID != x){
		int my_position = fai(&(lock->next_available_position));

		if((lock->current_position)!=my_position){//could not acquire lock on first try
			return 0;
		}
		else{// lock successfully acquired on first try
			lock->ownerID = x;
        	lock->recursive_counter = 0;
        	return 1;
		}
		
	}
	else{// lock successfully acquired on first try
		(lock->recursive_counter)++;
		return 1;
	}

	//return 0;

}
