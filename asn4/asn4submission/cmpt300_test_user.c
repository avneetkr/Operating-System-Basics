#include <stdio.h>
#include <linux/unistd.h>
#include <sys/syscall.h>

#define _cmpt300_TEST_ 327

int main(int argc, char *argv[]){

	printf("\nDiving to kernel level\n\n");
	syscall(_cmpt300_TEST_,300);
	printf("\nRising to user level\n\n");

	return 0;
} 
