#pragma once
/**
* CSC369 Assignment 2 - This file may be replaced when we run your code.
*
* This contains a utility for validating that something is actually being
* accessed exclusively by one thread at a time.
*/
#include "common.h"

struct _Car;

/**
* @brief Validates that something is being accessed exclusively by one thread
* at a time.
*/
typedef struct _MutexAccessValidator {

	/**
	* @brief Lock that guarantees safe validator operations.
	*/
	pthread_mutex_t lock;

	/**
	* @brief Pointer to the car currently in the validator.
	*/
	struct _Car* current;
} MutexAccessValidator;

/**
* @brief Initializes the given validator.
*
* @param validator pointer to the MutexAccessValidator to initialize.
*/
void initMutexAccessValidator(MutexAccessValidator* validator);

/**
* @brief Destructs the given validator.
*
* @param validator pointer to the MutexAccessValidator to destruct.
*/
void destructMutexAccessValidator(MutexAccessValidator* validator);

/**
* @brief Enters the mutex validator.
*
* Registers that this car is now inside the validator; if a car is already 
* inside, then there must be an error.
*
* @param self pointer to the mutex validator.
* @param car pointer to the car entering the validator.
*/
void enterMutexAccessValidator(MutexAccessValidator* self, 
		struct _Car* car);

/**
* @brief Exits the mutex validator.
*
* This allows a new thread to enter without an error being raised.
*
* @param self pointer to the mutex validator.
* @Param car pointer to the car exiting the validator.
*/
void exitMutexAccessValidator(MutexAccessValidator* self, 
			struct _Car* car);