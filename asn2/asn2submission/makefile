CC=gcc

all: thread_switching_timer process_switching_timer minimal-function-call-timer minimal-system-call-timer

thread_switching_timer: thread_switching_timer.c
	gcc -pthread thread_switching_timer.c -o thread_switching_timer

process_switching_timer: process_switching_timer.c
	gcc process_switching_timer.c -o process_switching_timer

minimal-function-call-timer: minimal-function-call-timer.c
	gcc minimal-function-call-timer.c -o minimal-function-call-timer

minimal-system-call-timer: minimal-system-call-timer.c
	gcc minimal-system-call-timer.c -o minimal-system-call-timer

clean:
	rm -f thread_switching_timer process_switching_timer minimal-function-call-timer minimal-system-call-timer


