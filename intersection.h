#pragma once
/**
* CSC369 Assignment 2 - This file may be replaced when we run your code.
*
* You may not access any struct members (except enums) from this file. All
* interactions with the intersections, etc. must be done through the given 
* function calls. We may also change the names or types of struct members 
* in the version we compile your code with.
*
* This contains things that are common to traffic lights and stop signs,
* such as tokens and lanes.
*/
#include <assert.h>
#include "mutexAccessValidator.h"
#include "car.h"

/**
* @brief Number of directions in an intersection.
*/
#define DIRECTION_COUNT 4

/**
* @brief Callback (function pointer) for events in an intersection.
*
* The traffic light intersection provides a couple of these callback hooks
* where you can provide your own functions to perform custom actions.
*
* @param userPtr pointer to anything you want. You provide this pointer as an
*   argument to the go-through-intersection function, and this is provided 
*   back as an argument to your callback function.
*/
typedef void(*IntersectionCallback)(void* userPtr);

/**
* @brief Helper struct for confirming that a car correctly navigated a 
* traffic scenario.
*
* We can fill these in when a car exits, enters, etc. an intersection or
* lane, providing a record of those actions.
*/
typedef struct _CarToken {

	/**
	* @brief Copy of the car data structure.
	*
	* We can check this later to ensure the Car wasn't corrupted or 
	* tampered with.
	*/
	Car carCopy;

	/**
	* @brief Token value provided.
	*/
	int token;

	/**
	* @brief Whether or not this is a valid CarToken.
	*/
	bool valid;

} CarToken;

/**
* @brief A lane that a car can enter the intersection from.
*
* Lanes are used to track the order in which cars enter and exit an
* intersection. Cars enter an intersection from a particular lane,
* and regardless of the action a car takes:
*   - Any car that entered from the same lane before it, must exit 
*       before it.
*   - Any car that entered from the same lane after it, must exit
*       after it.
*
* Call enterLane(...) before entering the intersection and call exitLane(...) 
* after exiting the intersection.
*/
typedef struct _EntryLane {

	/**
	* @brief Array of entry tokens, one for each car in the simulation, that 
	* records that the car entered the lane.
	*
	* This is over-provisioned. Many entries may be invalid at the end of the
	* simulation, since not all cars will enter this lane.
	*/
	CarToken* enterTokens;

	/**
	* @brief Array of car tokens, one for each car in the simulation, that
	* records that the car exited the lane.
	*
	* This is over-provisioned. Many entries may be invalid at the end of the
	* simulation, since not all cars will enter this lane.
	*/
	CarToken* exitTokens;

	/**
	* @brief Counter for assigning tokens on entry.
	*/
	int enterCounter;

	/**
	* @brief Counter for verifying exit order.
	*
	* We know a car exited in the same order it entered if the exit token it
	* gets is the same as the enter token it holds.
	*/
	int exitCounter;

} EntryLane;

/**
* @brief Initializes a CarToken.
*
* @param carToken pointer to the carToken to initialize.
* @param car pointer to the car this is being assigned to.
* @param tokenValue the value to initialize the token with.
*/
void initToken(CarToken* carToken, Car* car, int tokenValue);

/**
* @brief Enter a lane.
*
* Call before entering an intersection. Synchronization should be used when
* calling this.
*
* @param car pointer to the car.
* @param lane pointer to the lane to enter into.
*/
void enterLane(Car* car, EntryLane* lane);

/**
* @brief Exit the intersection.
*
* This will verify that the car exited the intersection in the same order as
* it entered in its entry lane.
* 
* Call after going through the intersection. Synchronization should be used when
* calling this.
*
* @param car pointer to the car.
* @param lane pointer to the lane that the car entered from.
*/
void exitIntersection(Car* car, EntryLane* lane);