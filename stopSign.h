#pragma once
/**
* CSC369 Assignment 2 - This file may be replaced when we run your code.
*
* You may not access any struct members (expect enums) from this file. All
* interactions with the intersections, etc. must be done through the given
* function. We may also change the names or types of struct members
* and in the version we compile your code with.
*
* This contains the declaration for the stop sign type of intersection.
*/
#include <assert.h>
#include "mutexAccessValidator.h"
#include "intersection.h"
#include "car.h"

/**
* @brief Number of quadrants in a stop sign intersection.
*/
#define QUADRANT_COUNT 4

/**
* @brief A quadrant of a StopSign intersection.
*
* Each StopSign intersection is composed of 4 quadrants.
*/
typedef struct _IntersectionQuadrant {

	/**
	* @brief Validator that detects collisions between cars.
	*/
	MutexAccessValidator validator;
	
	/**
	* @brief Counter for assigning tokens.
	*/
	int tokenCounter;
	
	/**
	* @brief Array of car tokens, one for each car in the simulation, that
	* records that the car entered and exited the quadrant.
	*/
	CarToken* tokens;

} IntersectionQuad;

/**
* @brief Intersection controlled by a stop sign.
*/
typedef struct _StopSign {

	/**
	* @brief Lanes that a car enters from.
	*
	* Depending on its direction, a car enters one of four lanes.
	*/
	EntryLane entryLanes[DIRECTION_COUNT];

	/**
	* @brief The quadrants of the intersection.
	*
	* A car will drive through between 1 and 3 quadrants depending on where
	* it is going.
	*/
	IntersectionQuad quadrants[QUADRANT_COUNT];

} StopSign;

/**
* @brief Initializes the stop sign.
*
* @param stopSign pointer to the stop sign.
* @param carCount number of cars in the simulation.
*/
void initStopSign(StopSign* stopSign, int carCount);

/**
* @brief Destroy the stop sign.
*
* @param stopSign pointer to the stop sign.
*/
void destroyStopSign(StopSign* stopSign);

/**
* @brief Gets the index of the lane that a car belongs in.
*
* No synchronization is needed to use this function.
*
* @param car pointer to the car.
* @param intersection pointer to the stop sign intersection.
* @return index of the lane that a car belongs in. Although you are not
*   allowed to directly access an intersection's lanes yourself, and you
*   would normally use getLane(...), this is here in case you want to create
*   your own array that shadows the intersection's lane data structure.
*/
int getLaneIndex(Car* car);

/**
* @brief Gets the lane that a car belongs in.
*
* No synchronization is needed to use this function.
*
* @param car pointer to the car.
* @param intersection pointer to the stop sign intersection.
*/
EntryLane* getLane(Car* car, StopSign* intersection);

/**
* @brief Has the given car go through the indicated stop sign controlled
* intersection.
*
* This should be called with any necessary synchronization already
* in place. For example, any needed locks should be already be held. They
* can be released after this function returns.
*
* @param car pointer to the car that will enter the intersection. This
*   thread will block in this function call until the car exits the
*   intersection.
* @param intersection pointer to the intersection.
*/
void goThroughStopSign(Car* car, StopSign* intersection);

/**
* @brief Helper that gets the indices of the quadrants of a stop-sign intersection
* that a car must travel through.
*
* No synchronization is needed when you call this.
*
* @param car pointer to the car.
* @param quadrants pointer to a buffer of at least 3 ints where the indices
*   of the quadrants will be placed. They may appear in any order.
* @return number of quadrants that the car will travel through and effectively,
*   the valid length of quadrants.
*/
int getStopSignRequiredQuadrants(Car* car, int* quadrants);
