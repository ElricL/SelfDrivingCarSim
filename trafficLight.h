#pragma once
/**
* CSC369 Assignment 2 - This file may be replaced when we run your code.
*
* You may not access any struct members (except enums) from this file. All
* interactions with the intersections, etc. must be done through the given
* functions. We may also change the names or types of struct members
* in the version we compile your code with.
*
* This contains the declaration for the traffic light type of intersection.
*/
#include <assert.h>
#include "mutexAccessValidator.h"
#include "intersection.h"
#include "car.h"

/**
* @brief Number of lanes in a traffic light intersection.
*
* Four directions x three lanes for each direction: {LEFT, STRAIGHT, RIGHT}.
*/
#define TRAFFIC_LIGHT_LANE_COUNT 12

/**
* @brief Enumerates the directions allowed by the traffic light.
*/
typedef enum _LightState {

	// A number of cars from north and south may proceed. Cars from 
	// east and west must wait.
	NORTH_SOUTH = 0,

	// A number of cars from east and west may proceed. Cars from north 
	// and south must wait.
	EAST_WEST = 1,

	// No cars may proceed. This state lasts until the intersection is clear.
	RED = 2

} LightState;

/**
* @brief Intersection controlled by a traffic light.
*/
typedef struct _TrafficLight {

	/**
	* @brief Lanes that a car enters from.
	*
	* Depending on its direction and action, a car enters one of twelve lanes.
	* There are four directions that a car may enter from and from each
	* direction there is a dedicated left-turn lane, straight lane and
	* right-turn lane.
	*/
	EntryLane entryLanes[TRAFFIC_LIGHT_LANE_COUNT];

	/**
	* @brief Current direction allowed by the light or RED.
	*/
	LightState currentMode;

	/**
	* @brief Previous direction allowed by the light.
	*
	* This is never set to RED.
	*/
	LightState previousDirection;

	/**
	* @brief Lock used to protect data used in detecting vehicle collisions.
	*/
	pthread_mutex_t validationLock;

	/**
	* @brief Array of car tokens, one for each car in the simulation, that
	* records that the car entered and exited the light.
	*/
	CarToken* tokens;

	/**
	* @brief Counter for assigning tokens.
	*/
	int tokenCounter;

	/**
	* @brief Number of cars that may proceed until the light switches. This
	* is decremented when a car enters the intersection.
	*
	* This should always be 0 when in RED mode.
	*
	* Technically speaking to avoid a potential deadlock: when enabling a
	* direction to move, we need to avoid setting carsLeft to more than the
	* number of cars that still need to go from that direction. Otherwise, the
	* light will sit there forever waiting for more cars before it transitions
	*/
	int carsLeft;

	/**
	* @brief Number of cars currently inside the intersection.
	*
	* The light won't transition out of RED until this is 0.
	*/
	int carsInside;

	/**
	* @brief Total number of cars in the simulation moving from east or west
	* that haven't entered the intersection yet.
	*
	* This is used to prevent deadlock when transitioning the light.
	*/
	int totalEWLeft;

	/**
	* @brief Total number of cars in the simulation moving from north or south
	* that haven't entered the intersection yet.
	*
	* This is used to prevent deadlock when transitioning the light.
	*/
	int totalNSLeft;

	/**
	* @brief Number of cars going straight through the intersection from
	* each direction.
	*
	* Invariant: at any time, the two directions that are not (or were not, if
	* it is red for both East/West and North/South) allowed to go will have 0
	* straight counts.
	*
	* Indexed by CarPosition.
	*/
	int straightCounts[DIRECTION_COUNT];

} TrafficLight;

/**
* @brief Initialize a traffic light.
*
* @param light pointer to the traffic light intersection.
* @param eastWest total number of cars that will arrive from the east or west.
* @param northSouth total number of cars that will arrive from the north or 
*   south.
*/
void initTrafficLight(TrafficLight* light, int eastWest, int northSouth);

/**
* @brief Destroy the traffic light.
*
* @param light pointer to the traffic light.
*/
void destroyTrafficLight(TrafficLight* light);

/**
* @brief Gets the index of the lane that a car belongs in.
*
* No synchronization is needed to use this function.
*
* @param car pointer to the car.
* @return index of the lane that a car belongs in. Although you are not
*   allowed to directly access an intersection's lanes yourself, and you
*   would normally use getLane(...), this is here in case you want to create
*   your own array that follows the intersection's lane data structure.
*/
int getLaneIndexLight(Car* car);

/**
* @brief Gets the lane that a car belongs in.
*
* No synchronization is needed to use this function.
*
* @param car pointer to the car.
* @param intersection pointer to the stop sign intersection.
*/
EntryLane* getLaneLight(Car* car, TrafficLight* intersection);

/**
* @brief Enters the indicated traffic light intersection in preparation for
* performing an action.
*
* When its time to actually perform an action, call actTrafficLight(...).
* Unlike stop sign, this is split into two functions so that you can include
* left-turn logic.
*
* This should be called with any necessary synchronization already in place.
*
* @param car pointer to the car that will enter the intersection.
* @param intersection pointer to the intersection.
*/
void enterTrafficLight(Car* car, TrafficLight* intersection);

/**
* @brief Has the given car peform its action in the given traffic light
* intersection, having already entered via enterTrafficLight(...).
*
* This should be called with any necessary synchronization already in place.
* You can read the implementation of this function and consider the scenario
* to decide how to write your synchronization code.
*
* @param car pointer to the car that will enter the intersection. This
*   thread will block in this function call until the car exits the
*   intersection.
* @param intersection pointer to the intersection.
* @param beforeSleep called after entry logic is run but before this thread
*   sleeps, which represents the time that the car proceeds through the
*   intersection. While the car is proceeding through the intersection, other
*   cars, including from the same lane, should be able to enter. You can
*   specify NULL to ignore.
* @param afterSleep called after this thread sleeps, but before exit logic is
*   run. You can specify NULL to ignore.
* @param userPtr the argument to provide to beforeSleep(...) and
*   afterSleep(...).
*/
void actTrafficLight(Car* car, TrafficLight* intersection,
	IntersectionCallback beforeSleep, IntersectionCallback afterSleep,
	void* userPtr);

/**
* @brief Helper that gets the opposite direction to the given one.
*
* @param mode the direction to get the opposite of, represented by a 
*   LightState. Must be either EAST_WEST or NORTH_SOUTH.
* @return LightState that represents the opposite direction to the one given.
*/
LightState getOppositeDirection(LightState mode);

/**
* @brief Gets the state of the given traffic light.
*
* You must ensure proper synchronization is in place when calling this.
*
* @param light pointer to the traffic light intersection.
* @return mode indicates from which direction cars are allowed to move
*   or if RED.
*/
LightState getLightState(TrafficLight* light);

/**
* @brief Gets the number of cars that have entered the indicated traffic light
* intersection and are going straight, starting from the indicated position.
*
* You must ensure proper synchronization is in place when calling this.
*
* @param light pointer to the traffic light intersection.
* @param position the position to check for cars going straight from
*   (cast to int).
* @return number of cars in the intersection that originate from the indicated
*   position and are going straight.
*/
int getStraightCount(TrafficLight* light, int position);
