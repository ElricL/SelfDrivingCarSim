#pragma once
/**
* CSC369 Assignment 2 - This file may be replaced when we run your code.
*
* This contains misc helper functions.
*/
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

// Borrowed from Stack-Exchange:
// https://stackoverflow.com/questions/2670816/how-can-i-use-the-compile-time-constant-line-in-a-string
#define STRINGIZE(x) STRINGIZE2(x)
#define STRINGIZE2(x) #x
#define LINE_STRING STRINGIZE(__LINE__)

#define FALSE 0
#define TRUE 1

typedef int bool;

/**
* @brief Computes the minimum of two integers.
*
* @param a the first value.
* @param b the second value.
* @return min(a, b).
*/
int minA2(int a, int b);

/**
* @brief Computes the maximum of two integers.
*
* @param a the first value.
* @param b the second value.
* @return max(a, b).
*/
int maxA2(int a, int b);

/**
* @brief Puts the thread to sleep for at least the indicated amount of time.
*
* @param duration number of microseconds to put the thread to sleep for.
*/
void nap(int duration);

/**
* @brief Initalizes a mutex and does error checking.
*
* @param mutex pointer to the mutex to initialize.
*/
void initMutex(pthread_mutex_t* mutex);

/**
* @brief Initalizes a condition variable and does error checking.
*
* @param cond pointer to the condition variable to initialize.
*/
void initConditionVariable(pthread_cond_t* cond);

/**
* @brief Unlocks a mutex and does error checking.
*
* @param mutex pointer to the mutex to unlock.
*/
void unlock(pthread_mutex_t* mutex);