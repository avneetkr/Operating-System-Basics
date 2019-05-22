#include <linux/kernel.h>

asmlinkage long sys_cmpt300_test(int arg0){
	printk(" Hello World !");
	printk("--syscall arg %d",arg0);
	
	return((long) arg0);
}
