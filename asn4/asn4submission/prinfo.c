//********************************************************
// Creating prinfo system call
//
//
// Author: Avneet Kaur
// Date: April 7, 2017
//
//********************************************************
#include <linux/types.h>
#include <asm/uaccess.h> //for copy_from_user
#include <linux/sched.h> // for current()
#include <asm/current.h>
#include <linux/list.h> //contains list_head
//#include <string.h>
#include <asm/cputime.h>
#include <linux/kernel.h>
#include <linux/string.h>
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
	struct list_head* child_ptr = kmalloc(sizeof(struct list_head),GFP_KERNEL);
	struct task_struct* mytask = kmalloc(sizeof(struct task_struct),GFP_KERNEL);
	struct task_struct* child_task = kmalloc(sizeof(struct task_struct),GFP_KERNEL);
	struct task_struct* younger_sibling = kmalloc(sizeof(struct task_struct),GFP_KERNEL);
	struct task_struct*  older_sibling = kmalloc(sizeof(struct task_struct),GFP_KERNEL);
	pid_t max_pid;
	struct timespec starttime_struct;
	//char *comm_buffer;
	long total_utime_children, total_stime_children;
	
	if(info==NULL){
		printk("ERROR: NULL Argument. Exiting...\n");
		return -22;
	}
	else if(copy_from_user(&local_info->pid,&info->pid,sizeof(pid_t))) {
		printk("ERROR: Cannot copy from user. Exiting...\n");
		return -EFAULT;

	}
	printk("111111\n");
	mytask = pid_task(find_vpid(local_info->pid), PIDTYPE_PID);
	//mytask = current;
	printk("222222\n");
	//state
	local_info->state = mytask->state;
	//printk("333333\n");
	//nice value - subtracting from dynamic priority
	local_info->nice = (mytask->prio)-20-100;//-20; ///CONFIRMMMM
	//printk("444444\n");
	//parent_pid
	if(mytask->parent==NULL){
		printk("ERROR: NULL parent. Exiting...\n");
		return -22;
	}
	else{
		local_info->parent_pid = mytask->parent->pid;
	}
	
	//youngest_child_pid 
	//-----------------------------------------------------------
	//if(current->children==NULL){
	//	local_info->youngest_child_pid = -1;
	//}
	//else{
		// child_ptr: current ptr to that child process
		//	&mytask->children: head of children linked list
		//sibling: the sibling linked list in the children-head contains 
		// the siblings of that child process=>it contains the rest
		//of the children of our process
		max_pid = mytask->pid;
		total_stime_children = 0;
		total_utime_children = 0;
	//	printk("55555555\n");
		list_for_each(child_ptr,&mytask->children){
			//if(child_ptr == NULL){
			//	return -22;
			//}

			child_task = list_entry(child_ptr,struct task_struct, sibling);
			
			//if(child_task == NULL){
			//	return -22;
			//}
			//finding max pid
			if(child_task->pid > max_pid){
				max_pid = child_task->pid;
			}
			//adding user time & system time for that child to total
			total_utime_children+=(long)cputime_to_msecs(child_task->utime); 
			total_stime_children+= (long)cputime_to_msecs(child_task->stime);
		}
	//	printk("6666666\n");

		//The max_pid will be the pid of the youngest child
		local_info->youngest_child_pid = max_pid;
		if(max_pid == mytask->pid){ //No children
			local_info->youngest_child_pid = -1;

		}
	//	printk("777777777\n");
	//}
	//-----------------------------------------------------------

	//younger_sibling_pid
	younger_sibling = list_entry((mytask->sibling.next), struct  task_struct, sibling);
	local_info->younger_sibling_pid = younger_sibling->pid; 
	if(local_info->younger_sibling_pid == 0){ //no younger sibling
		local_info->younger_sibling_pid = -1;
		
	}
	
	//older_sibling_pid
	older_sibling = list_entry((mytask->sibling.prev), struct task_struct, sibling);
	local_info->older_sibling_pid = older_sibling->pid;
	if(local_info->older_sibling_pid == 0){ //no older sibling
		local_info->older_sibling_pid = -1;
	}
	//printk("8888888888\n");
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
	
	starttime_struct = mytask->start_time;
	local_info->start_time = (unsigned long)((starttime_struct.tv_sec)*1000) + (unsigned long)((starttime_struct.tv_nsec)/1000000);
	//printk("99999999\n");
	// cputime_t is in nanoseconds
	// cputime_to_msecs converts nanoseconds to milliseconds
	local_info->user_time = (long)cputime_to_msecs(mytask->utime); 
	local_info->sys_time = (long)cputime_to_msecs(mytask->stime);

	//total user time of children
	local_info->cutime = total_utime_children;

	//total system time of children
	local_info->cstime = total_stime_children; 

	// user id
	local_info->uid = (long)(mytask->uid);

	//safely copy program name
	//strncpy(local_info->comm,current->comm);
	char *buf = kmalloc(16*sizeof(char), GFP_KERNEL);
	strncpy(local_info->comm, get_task_comm(buf,mytask),sizeof(local_info->comm)-1);
	(local_info->comm)[15] = '\0';
	//printk("MMMMM NAME: %s\n",local_info->comm); 
	//strnpy(local_info,buf,sizeof());
	//printk("AAAAAAAAAAAAAAAAAAAA\n");
	//copy results to user space
	if(copy_to_user(&info->state,&local_info->state,sizeof(long))) return -EFAULT;
	if(copy_to_user(&info->nice,&local_info->nice,sizeof(long))) return -EFAULT;
	if(copy_to_user(&info->parent_pid,&local_info->parent_pid,sizeof(pid_t))) return -EFAULT;
	if(copy_to_user(&info->youngest_child_pid,&local_info->youngest_child_pid,sizeof(pid_t))) return -EFAULT;
	if(copy_to_user(&info->younger_sibling_pid,&local_info->younger_sibling_pid,sizeof(pid_t))) return -EFAULT;
	if(copy_to_user(&info->older_sibling_pid,&local_info->older_sibling_pid,sizeof(pid_t))) return -EFAULT;
	if(copy_to_user(&info->start_time,&local_info->start_time,sizeof(unsigned long))) return -EFAULT;
	if(copy_to_user(&info->user_time,&local_info->user_time,sizeof(long))) return -EFAULT;
	if(copy_to_user(&info->sys_time,&local_info->sys_time,sizeof(long))) return -EFAULT;
	if(copy_to_user(&info->cutime,&local_info->cutime,sizeof(long))) return -EFAULT;
	if(copy_to_user(&info->cstime,&local_info->cstime,sizeof(long))) return -EFAULT;
	if(copy_to_user(&info->uid,&local_info->uid,sizeof(long))) return -EFAULT;
	if(copy_to_user(&info->comm,&local_info->comm,sizeof(local_info->comm))) return -EFAULT;
	//printk("BBBBBBBBBBBBBBBBBBBB\n");
	kfree(local_info);
	//printk("CCCCCCCCCCCCCCCC\n");
	//kfree(child_ptr);
	//printk("DDDDDDDDDDDDDDD\n");
	kfree(mytask);
	//printk("EEEEEEEEEEE\n");
	kfree(child_task);
	//printk("FFFFFFFFFFFF\n");
	//kfree(younger_sibling);
	//printk("GGGGGGGGGGGGG\n");
	kfree(older_sibling);
	//printk("HHHHHHHHHHHHHHC\n");
	return 0;



}
