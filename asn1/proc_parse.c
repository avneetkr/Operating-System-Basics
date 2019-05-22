#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){

FILE *fp;

char *nextline = malloc(400); 
char cpuinfo_file[] = "/proc/cpuinfo";
char modelname[] = "model name";

char version_file[] = "/proc/version";

char meminfo_file[] = "/proc/meminfo";
char memtotal[] = "MemTotal";	

char uptime_file[] = "/proc/uptime";
	

// Processor Type
fp = fopen(cpuinfo_file, "r");
if (fp == NULL){
	fprintf(stderr, "Can't open cpuinfo file \n");
	exit(1);
}
while(fgets(nextline, 100, fp)!=NULL) {
	if(strstr(nextline, modelname) != NULL) {
		printf("CPU %s\n", nextline);
		break;
	}
}
fclose(fp);

// Kernel Version
fp = fopen(version_file,"r");
if (fp == NULL){
	fprintf(stderr,"Can't open version file \n");
	exit(1);
}
fgets(nextline, 150, fp);
printf("Kernel Version:	%s\n", nextline);
fclose(fp);

// Amount of memory configured into the computer
fp = fopen(meminfo_file, "r");
if (fp == NULL){
	fprintf(stderr, "Can't open meminfo file \n");
	exit(1);
}
while(fgets(nextline, 100, fp)!=NULL) {
	if(strstr(nextline, memtotal) != NULL) {
		printf("\n%s\n", nextline);
		break;
	}
}
fclose(fp);

// Amount of time since the system was last booted
fp = fopen(uptime_file,"r");
if (fp == NULL){
	fprintf(stderr,"Can't open uptime file \n");
	exit(1);
}
fgets(nextline, 100, fp);
printf("Uptime: 	%s seconds\n", strtok(nextline," "));
fclose(fp);
free(nextline);


return(1);

}
