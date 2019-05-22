/*
 * Avneet Kaur 
 * avneetk@sfu.ca
 * 301209987
 *
 * sync.h
 */

#ifndef _my_SYNC1_H_
#define _my_SYNC1_H_

#include "atomic_ops.h"


#define MIN_DELAY 2
#define MAX_DELAY 100

struct my_mutex_struct {
	volatile long unsigned int lock_value;
    volatile long unsigned int *lockptr;
   
    int delay;
    int ownerID;
	int recursive_counter;
};




typedef struct my_mutex_struct my_mutex_t;


int my_mutex_init(my_mutex_t *lock);
int my_mutex_unlock(my_mutex_t *lock);
int my_mutex_destroy(my_mutex_t *lock);

int my_mutex_lock(my_mutex_t *lock);
int my_mutex_trylock(my_mutex_t *lock);




/*Spinlock Starts here*/

struct my_spinlock_struct {
	volatile long unsigned int lock_value; 
	volatile long unsigned int* lockptr;
	int ownerID;
	int recursive_counter;
};

typedef struct my_spinlock_struct my_spinlock_t;

int my_spinlock_init(my_spinlock_t *lock);
int my_spinlock_destroy(my_spinlock_t *lock);
int my_spinlock_unlock(my_spinlock_t *lock);

int my_spinlock_lockTAS(my_spinlock_t *lock);
int my_spinlock_lockTTAS(my_spinlock_t *lock);
int my_spinlock_trylock(my_spinlock_t *lock);




/*queuelock Starts here*/

//struct node{
//	int position;
//	node *next;
//};

struct my_queuelock_struct {
  
  volatile int lock_value;
  //volatile int *lock;

  //node *head;//int *current_position;//now_serving
  //node *tail;//int next_available_position;
  //int size;
  volatile unsigned long current_position;
  volatile unsigned long next_available_position;
  int ownerID;
  int recursive_counter;
};

typedef struct my_queuelock_struct my_queuelock_t;

int my_queuelock_init(my_queuelock_t *lock);
int my_queuelock_destroy(my_queuelock_t *lock);
int my_queuelock_unlock(my_queuelock_t *lock);

int my_queuelock_lock(my_queuelock_t *lock);
int my_queuelock_trylock(my_queuelock_t *lock);


#endif
