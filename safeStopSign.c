/**
* CSC369 Assignment 2
*
* This is the source/implementation file for your safe stop sign
* submission code.
*/
#include "safeStopSign.h"

void initSafeStopSign(SafeStopSign* sign, int count) {
	initStopSign(&sign->base, count);

    // Initializes locks for quadrants
    for (int i=0; i<QUADRANT_COUNT; i++){
        pthread_mutex_init(&sign->quad_lock[i], NULL);
    }
    // Initialize each lane's mutex, condition variable, and car list.
    for (int i = 0; i < DIRECTION_COUNT; i++) {
        pthread_mutex_init(&sign->lanes[i].mutex, NULL);
		pthread_cond_init(&sign->lanes[i].turn, NULL);
		sign->lanes[i].carsWaiting = NULL;
    }
}

void destroySafeStopSign(SafeStopSign* sign) {
	destroyStopSign(&sign->base);

	// Destroy all condition variables and locks
    for (int i = 0; i < DIRECTION_COUNT; i++) {
		pthread_cond_destroy(&sign->lanes[i].turn);
		pthread_mutex_destroy(&sign->lanes[i].mutex);
    }
    for (int i = 0; i<QUADRANT_COUNT; i++){
        pthread_mutex_destroy(&sign->quad_lock[i]);
    }
}

void runStopSignCar(Car* car, SafeStopSign* sign) {
    int carLane = getLaneIndex(car);

	EntryLane* lane = getLane(car, &sign->base);

	// Only one car can enter a a time
	pthread_mutex_lock(&sign->lanes[carLane].mutex);
	enterLane(car, lane);

	// Create order entry of the car and store it
	if (sign->lanes[carLane].carsWaiting == NULL) {
        // Add car at beginning of the line
        sign->lanes[carLane].carsWaiting = malloc(sizeof(CarOrder));
        sign->lanes[carLane].carsWaiting->index = car->index;
        sign->lanes[carLane].carsWaiting->next = NULL;
	} else {
	    // Add car at the back of the line
	    CarOrder *cur = sign->lanes[carLane].carsWaiting;
        while (cur->next != NULL) {
            cur = cur->next;
        }
        cur->next = malloc(sizeof(CarOrder));
        cur->next->index = car->index;
        cur->next->next = NULL;
	}
	pthread_mutex_unlock(&sign->lanes[carLane].mutex);

	// Get information on quadrants to avoid collision
	int quadrants[QUADRANT_COUNT];
	int quadrantCount = getStopSignRequiredQuadrants(car, quadrants);

    // Wait if quadrant is filled, otherwise retrieve the lock
    for (int i = 0; i < quadrantCount; i++){
            pthread_mutex_lock(&sign->quad_lock[i]);
    }

	goThroughStopSign(car, &sign->base);

	// Free locks after going through quadrants
    for (int i = 0; i < quadrantCount; i++){
            pthread_mutex_unlock(&sign->quad_lock[i]);
    }

    //Exit in same order as entering
    pthread_mutex_lock(&sign->lanes[carLane].mutex);
    while(sign->lanes[carLane].carsWaiting->index != car->index) {
        // Wait if not car is not in front of the line line
        pthread_cond_wait(&sign->lanes[carLane].turn, &sign->lanes[carLane].mutex);
    }


	exitIntersection(car, lane);

	// Update the waiting list and tell next car in the lane to go next
	CarOrder *carFree = sign->lanes[carLane].carsWaiting ;
	sign->lanes[carLane].carsWaiting = sign->lanes[carLane].carsWaiting->next;
	free(carFree);
	pthread_cond_broadcast(&sign->lanes[carLane].turn);
	pthread_mutex_unlock(&sign->lanes[carLane].mutex);
}
