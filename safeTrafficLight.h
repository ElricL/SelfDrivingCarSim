#pragma once
/**
* CSC369 Assignment 2
*
* This is the header file for your safe traffic light submission code.
*/
#include "car.h"
#include "trafficLight.h"

/**
* Order entry of a car identified by the car's index
*/
typedef struct _CarOrder2 {
    int index; // Index of car
    struct _CarOrder2 *next;
} CarOrder2;


/**
* Lane that tracks entry of cars, lock and condition variables
* for synchronization
*/
typedef struct lane2 {
    // Synchronization for lane
    pthread_mutex_t mutex; // used to enter and exit lane
    // List of cars waiting to pass through the intersection
    CarOrder2 *carsWaiting;
    pthread_cond_t turn; // CV for checking if it's car's turn to go
} Lane2;
/**
* @brief Structure that you can modify as part of your solution to implement
* proper synchronization for the traffic light intersection.
*
* This is basically a wrapper around TrafficLight, since you are not allowed to
* modify or directly access members of TrafficLight.
*/
typedef struct _SafeTrafficLight {

	/**
	* @brief The underlying light.
	*
	* You are not allowed to modify the underlying traffic light or directly
	* access its members. All interactions must be done through the functions
	* you have been provided.
	*/
	TrafficLight base;

    pthread_mutex_t light_mutex; // lock for Traffic light
    pthread_cond_t lightEW; // CV to indicate green light for East/West lanes
    pthread_cond_t lightSN; // CV to indicate green light for South/North lanes
    pthread_cond_t no_straight; // CV to indicate no straights for left turning cars

    // All lanes to keep track of cars and lane synchronization
    Lane2 lanes[TRAFFIC_LIGHT_LANE_COUNT];

} SafeTrafficLight;

/**
* @brief Initializes the safe traffic light.
*
* @param light pointer to the instance of SafeTrafficLight to be initialized.
* @param eastWest total number of cars moving east-west.
* @param northSouth total number of cars moving north-south.
*/
void initSafeTrafficLight(SafeTrafficLight* light, int eastWest, int northSouth);

/**
* @brief Destroys the safe traffic light.
*
* @param light pointer to the instance of SafeTrafficLight to be destroyed.
*/
void destroySafeTrafficLight(SafeTrafficLight* light);

/**
* @brief Runs a car-thread in a traffic light scenario.
*
* @param car pointer to the car.
* @param light pointer to the traffic light intersection.
*/
void runTrafficLightCar(Car* car, SafeTrafficLight* light);
