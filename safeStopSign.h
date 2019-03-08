#pragma once
/**
* CSC369 Assignment 2
*
* This is the header file for your safe stop sign submission code.
*/
#include "car.h"
#include "stopSign.h"

/**
* Order entry of a car identified by the car's index
*/
typedef struct _CarOrder {
    int index; // Index of car
    struct _CarOrder *next;
} CarOrder;

/**
* Lane that tracks entry of cars, lock and condition variables
* for synchronization on entering and exiting lanes
*/
typedef struct lane {
    // Synchronization for lane
    pthread_mutex_t mutex;
    pthread_cond_t  turn;

    // The order of the cars that will exit the intersection
    // The front of the linked list is the front of the line
    CarOrder *carsWaiting;
} Lane;

/**
* @brief Structure that you can modify as part of your solution to implement
* proper synchronization for the stop sign intersection.
*
* This is basically a wrapper around StopSign, since you are not allowed to
* modify or directly access members of StopSign.
*/
typedef struct _SafeStopSign {

	/**
	* @brief The underlying stop sign.
	*
	* You are not allowed to modify the underlying stop sign or directly
	* access its members. All interactions must be done through the functions
	* you have been provided.
	*/
	StopSign base;

	// Locks for quadrants
    pthread_mutex_t quad_lock[QUADRANT_COUNT];

    // All lanes for each direction
    Lane lanes[DIRECTION_COUNT];

} SafeStopSign;

/**
* @brief Initializes the safe stop sign.
*
* @param sign pointer to the instance of SafeStopSign to be initialized.
* @param carCount number of cars in the simulation.
*/
void initSafeStopSign(SafeStopSign* sign, int carCount);

/**
* @brief Destroys the safe stop sign.
*
* @param sign pointer to the instance of SafeStopSign to be freed
*/
void destroySafeStopSign(SafeStopSign* sign);

/**
* @brief Runs a car-thread in a stop-sign scenario.
*
* @param car pointer to the car.
* @param sign pointer to the stop sign intersection.
*/
void runStopSignCar(Car* car, SafeStopSign* sign);
