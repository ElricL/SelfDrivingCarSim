/**
* CSC369 Assignment 2 - This file may be replaced when we run your code.
*
* Implementation of the MutexAccessValidator.
*/
#include "mutexAccessValidator.h"

void initMutexAccessValidator(MutexAccessValidator* validator) {
	initMutex(&validator->lock);
	validator->current = NULL;
}


void destructMutexAccessValidator(MutexAccessValidator* validator) {
	pthread_mutex_destroy(&validator->lock);
}

void enterMutexAccessValidator(MutexAccessValidator* self, 
		struct _Car* car) {
	pthread_mutex_lock(&self->lock);
	if (self->current != NULL) {
		fprintf(stderr, "Collision!\n"\
				"@ " __FILE__ " : " LINE_STRING "\n");
	} else {
		self->current = car;
	}
	unlock(&self->lock);
}

void exitMutexAccessValidator(MutexAccessValidator* self, 
			struct _Car* car) {
	pthread_mutex_lock(&self->lock);
	
	// If a collision happened, this won't make so much sense.
	self->current = NULL;

	unlock(&self->lock);
}