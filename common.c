/**
* CSC369 Assignment 2 - This file may be replaced when we run your code.
*
* Implementation of the various misc helper functions from Common.h.
*/
#include "errno.h"
#include "common.h"

int minA2(int a, int b) {
	return a < b ? a : b;
}

int maxA2(int a, int b) {
	return a > b ? a : b;
}

void nap(int duration) {
	
	if (duration < 0) {
		return;
	}
	
	struct timespec spec;
	spec.tv_sec = 0;
	spec.tv_nsec = duration * 1000;
	
	struct timespec remainder;
	
	// Keep repeating until its slept for at least the indicated amount of time.
	int result = -1;
	do {
		
		result = nanosleep(&spec, &remainder);
		if (result == -1) {
			if (errno != EINTR) { 
				perror("Nanosleep encountered an error"
						"@ " __FILE__ " : " LINE_STRING "\n");
				return;
			} else {
				spec = remainder;
			}
		}
		
	} while (result == -1);
}

void initMutex(pthread_mutex_t* mutex) {
	int returnValue = pthread_mutex_init(mutex, NULL);
	if (returnValue != 0) {
		perror("Mutex initialization failed."
				"@ " __FILE__ " : " LINE_STRING "\n");	
	}
}

void initConditionVariable(pthread_cond_t* cond) {
	int returnValue = pthread_cond_init(cond, NULL);
	if (returnValue != 0) {
		perror("Condition variable initialization failed."
				"@ " __FILE__ " : " LINE_STRING "\n");	
	}
}

void unlock(pthread_mutex_t* mutex) {
	int returnValue = pthread_mutex_unlock(mutex);
	if (returnValue != 0) {
		perror("Mutex unlock failed."
				"@ " __FILE__ " : " LINE_STRING "\n");	
	}
}