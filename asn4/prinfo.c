/********************************************************
// Creating prinfo system call
//
//
// Author: Avneet Kaur
// Date: April 7, 2017
//
//********************************************************/
#include <linux/types.h>
#include <asm/uaccess.h> //for copy_from_user
#include <linux/sched.h> // for current()
#include <asm/current.h>
#include <linux/list.h> //contains list_head
//#include <string.h>
#include <asm/cputime.h>
#include <linux/kernel.h>
#include "linux/prinfo.h"


// ************************************************************
// prinfo SYSTEM CALL
//----------------------------------------------------------
// returns value of prinfo attributes for the process specified 
// by info->pid
// if info is NULL, returns -EINVAL
// returns 0 on success
//*************************************************************
asmlinkage long sys_prinfo(struct prinfo* info){
	struct prinfo* local_info = kmalloc(sizeof(struct prinfo), GFP_KERNEL);
	struct list_head* child_ptr;
	struct task_struct* child_task;
	struct task_struct* younger_sibling;
	struct task_struct*  older_sibling;
	pid_t max_pid;
	struct timespec starttime_struct;
	//char *comm_buffer;
	long total_utime_children, total_stime_children;
	
	if(info==NULL){
		return -22;
	}
	//else if(copy_from_user(local_info,info,sizeof(prinfo *))) {
	//	return -EFAULT;

	//}

	
	//state
	local_info->state = current->state;
	
	//nice value - subtracting from dynamic priority
	local_info->nice = (current->prio)-20;//-100-20; ///CONFIRMMMM
	
	//parent_pid
	if(current->parent==NULL){
		return -22;
	}
	else{
		local_info->parent_pid = current->parent->pid;
	}
	
	//youngest_child_pid 
	//-----------------------------------------------------------
	//if(current->children==NULL){
	//	local_info->youngest_child_pid = -1;
	//}
	//else{
		// child_ptr: current ptr to that child process
		//	&current->children: head of children linked list
		//sibling: the sibling linked list in the children-head contains 
		// the siblings of that child process=>it contains the rest
		//of the children of our process
		max_pid = current->pid;
		total_stime_children = 0;
		total_utime_children = 0;

		list_for_each(child_ptr,&current->children){
			if(child_ptr == NULL){
				return -22;
			}

			child_task = list_entry(child_ptr,struct task_struct, sibling);
			
			if(child_task == NULL){
				return -22;
			}
			//finding max pid
			if(child_task->pid > max_pid){
				max_pid = child_task->pid;
			}
			//adding user time & system time for that child to total
			total_utime_children+=(long)cputime_to_msecs(child_task->utime); 
			total_stime_children+= (long)cputime_to_msecs(child_task->stime);
		}

		//The max_pid will be the pid of the youngest child
		local_info->youngest_child_pid = max_pid;
		if(max_pid == current->pid){ //No children
			local_info->youngest_child_pid = -1;

		}
	//}
	//-----------------------------------------------------------

	//younger_sibling_pid
	younger_sibling = list_entry((current->sibling.next), struct  task_struct, sibling);
	local_info->younger_sibling_pid = younger_sibling->pid; 
	if(local_info->younger_sibling_pid == 0){ //no younger sibling
		local_info->younger_sibling_pid = -1;
		
	}

	//older_sibling_pid
	older_sibling = list_entry((current->sibling.prev), struct task_struct, sibling);
	local_info->older_sibling_pid = older_sibling->pid;
	if(local_info->older_sibling_pid == 0){ //no older sibling
		local_info->older_sibling_pid = -1;
	}

	//start_time
	//
	//The struct timespec structure represents an elapsed time. It is declared in time.h and has the following members:
	//time_t tv_sec
	//     This represents the number of whole seconds of elapsed time.
	//long int tv_nsec
	//This is the rest of the elapsed time (a fraction of a second), represented as the number of nanoseconds. 
	// Total time in milliseconds:
	// Seconds to mSeconds: tv_sec*1000 +
	// nSeconds to mSeconds: tv_sec/1000000
	
	starttime_struct = current->start_time;
	local_info->start_time = (unsigned long)((starttime_struct.tv_sec)*1000) + (unsigned long)((starttime_struct.tv_nsec)/1000000);

	// cputime_t is in nanoseconds
	// cputime_to_msecs converts nanoseconds to milliseconds
	local_info->user_time = (long)cputime_to_msecs(current->utime); 
	local_info->sys_time = (long)cputime_to_msecs(current->stime);

	//total user time of children
	local_info->cutime = total_utime_children;

	//total system time of children
	local_info->cstime = total_stime_children; 

	// user id
	local_info->uid = (long)(current->uid);

	//safely copy program name
	//strcpy(local_info->comm,current->comm);
	get_task_comm(local_info->comm,current);

	//copy results to user space
	copy_to_user(info,local_info,sizeof(struct prinfo*));
	return 0;



}
