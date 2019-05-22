#include <stdio.h>
#include <linux/unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>
#define _prinfo_ 328

struct prinfo {
long state; /* current state of process */
long nice; /* process nice value */
pid_t pid; /* process id (input) */
pid_t parent_pid; /* process id of parent */
pid_t youngest_child_pid; /* process id of youngest child */
pid_t younger_sibling_pid; /* pid of the oldest among younger siblings */
pid_t older_sibling_pid; /* pid of the youngest among older siblings */
unsigned long start_time; /* process start time */
long user_time; /* CPU time spent in user mode */
long sys_time; /* CPU time spent in system mode */
long cutime; /* total user time of children */
long cstime; /* total system time of children */
long uid; /* user id of process owner */
char comm[16]; /* name of
program executed */
};

int main(int argc, char *argv[]){

        struct prinfo* mytaskinfo = malloc(sizeof(struct prinfo));
        mytaskinfo->mypid = getpid();
        printf("\nDiving to kernel level\n\n");
        syscall(_prinfo_,mytaskinfo);

        printf("\nRising to user level\n\n");
        printf("My Task's Info: \n");
        printf("	------------------------------------------------------\n");
        printf("	State: 				%lu\n",mytaskinfo->state);
        printf("	Niceness:			%lu\n",mytaskinfo->nice );
        printf("	pid:				%d\n",mytaskinfo->pid );
        printf("	parent_pid:			%d\n",mytaskinfo->parent_pid);
        printf("	youngest_child_pid:	%d\n",mytaskinfo->youngest_child_pid);
        printf("	younger_sibling_pid:%d\n",mytaskinfo->younger_sibling_pid);
        printf("	older_sibling_pid:	%d\n",mytaskinfo->older_sibling_pid);
        printf("	------------------------------------------------------\n");
        printf("	Start Time:			%lu ms\n", mytaskinfo->start_time);
        printf("	User Time:			%lu ms\n", mytaskinfo->user_time);
        printf("	System Time:		%lu ms\n",mytaskinfo->sys_time );
        printf("	Total User Time of");
        printf("       children:		%lu ms\n", mytaskinfo->cutime);
        printf("	Total System Time");
        printf("       of children:		%lu ms\n", mytaskinfo->cutime);
        printf("	------------------------------------------------------\n");
        printf("	User ID:			%lu\n", mytaskinfo->uid);
        printf("	Name:				%s\n",mytaskinfo->comm);
        printf("	------------------------------------------------------\n");

        return 0;
}