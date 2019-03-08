#pragma once
/**
* CSC369 Assignment 2 - This file may be replaced when we run your code.
*
* This specifies information about a car and the data it holds. It also
* contains the functions that each car-thread will run.
*
* Each car is controlled by a single thread.
*/
#include <pthread.h>

/**
* @brief Enumerates the different positions that a car can start from in an
* intersection.
*/
typedef enum _CarPosition {
	EAST = 0,
	NORTH = 1,
	WEST = 2,
	SOUTH = 3
} CarPosition;

/**
* @brief Enumerates the different actions a car can take.
*/
typedef enum _CarAction {
	STRAIGHT = 0,
	RIGHT_TURN = 1,
	LEFT_TURN = 2,

	// Maybe another year: U_TURN
} CarAction;

/**
* @brief A car in this simulation.
* 
* Each car is controlled by one thread.
*/
typedef struct _Car {

	/**
	* @brief Index of this car in the simulation array of cars.
	*/
	int index;

	/**
	* @brief The position in the intersection that a car starts from.
	*/
	CarPosition position;

	/**
	* @brief Action that this car will take when it goes through the
	* intersection.
	*/
	CarAction action;

	/**
	* @brief Optional pointer that you can use to augment this struct with
	* your own data.
	*
	* Remember that in C, one thread is not allowed to access data allocated
	* on the stack of another.
	*/
	void* userPtr;

} Car;

/**
* @brief Helper gets the position opposite to the indicated position.
*
* @return the position opposite to the given position. For example, if given
*   NORTH, this returns SOUTH.
*/
CarPosition getOppositePosition(CarPosition position);

/**
* @brief Initializes the given car.
*
* @param car pointer to the car to initialize.
* @param index the index of the car in the array of cars.
* @param position the position that the car starts off in relative to the
*   intersection. A car that is SOUTH will travel north into the 
*   intersection.
* @param action the action the car will take once it reaches the 
*   intersection.
*/
void initCar(Car* car, int index, CarPosition position, CarAction action);