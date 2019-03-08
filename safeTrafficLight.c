/**
* CSC369 Assignment 2
*
* This is the source/implementation file for your safe traffic light
* submission code.
*/
#include "safeTrafficLight.h"

void initSafeTrafficLight(SafeTrafficLight* light, int horizontal, int vertical) {
	initTrafficLight(&light->base, horizontal, vertical);

    // Initialize each lane's mutex, condition variable, and car list.
    pthread_mutex_init(&light->light_mutex, NULL);
    pthread_cond_init(&light->lightEW, NULL);
    pthread_cond_init(&light->lightSN, NULL);
    pthread_cond_init(&light->no_straight, NULL);

    for (int i = 0; i < TRAFFIC_LIGHT_LANE_COUNT; i++) {
        pthread_mutex_init(&light->lanes[i].mutex, NULL);
        pthread_cond_init(&light->lanes[i].turn, NULL);
        light->lanes[i].carsWaiting = NULL;
    }
}

void destroySafeTrafficLight(SafeTrafficLight* light) {
	destroyTrafficLight(&light->base);

    pthread_mutex_destroy(&light->light_mutex);
    pthread_cond_destroy(&light->lightEW);
    pthread_cond_destroy(&light->lightSN);
    pthread_cond_destroy(&light->no_straight);

    for (int i = 0; i < TRAFFIC_LIGHT_LANE_COUNT; i++) {
        pthread_mutex_destroy(&light->lanes[i].mutex);
        pthread_cond_destroy(&light->lanes[i].turn);
    }
}

void runTrafficLightCar(Car* car, SafeTrafficLight* light) {
	int carLane = getLaneIndexLight(car);

    // Only one car can enter a a time
	pthread_mutex_lock(&light->lanes[carLane].mutex);
	EntryLane* lane = getLaneLight(car, &light->base);
	enterLane(car, lane);

    // Create order entry of the car and store it in waiting list
	if (light->lanes[carLane].carsWaiting == NULL) {
        // Add car at beginning of the line
        light->lanes[carLane].carsWaiting = malloc(sizeof(CarOrder2));
        light->lanes[carLane].carsWaiting->index = car->index;
        light->lanes[carLane].carsWaiting->next = NULL;
	} else {
	    // Add car at end of the line
	    CarOrder2 *cur = light->lanes[carLane].carsWaiting;
        while (cur->next != NULL) {
            cur = cur->next;
        }
        cur->next = malloc(sizeof(CarOrder2));
        cur->next->index = car->index;
        cur->next->next = NULL;
	}
    pthread_mutex_unlock(&light->lanes[carLane].mutex);

    //Enter when light is green for corresponding lanes
    pthread_mutex_lock(&light->light_mutex);
    if (car->position == EAST || car->position == WEST) {
        while (getLightState(&light->base) != EAST_WEST) {
            // Wait for light to turn green for EAST/WEST lanes
            pthread_cond_wait(&light->lightEW, &light->light_mutex);
        }
    } else {
        while (getLightState(&light->base) != NORTH_SOUTH) {
            // Wait for light to turn green for SOUTH/NORTH lanes
            pthread_cond_wait(&light->lightSN, &light->light_mutex);
        }
    }

	// Enter and act are separate calls because a car turning left can first
	// enter the intersection before it needs to check for oncoming traffic.
	enterTrafficLight(car, &light->base);

    if (car->action == LEFT_TURN) {
        CarPosition opposite = getOppositePosition(car->position);
        while (getStraightCount(&light->base, opposite) > 0) {
            // Wait until there no more cars going straight to safely turn left
            pthread_cond_wait(&light->no_straight, &light->light_mutex);
        }
    }

	actTrafficLight(car, &light->base, NULL, NULL, NULL);

	//Let left turn cars on opposite side know there no more cars going straight
   if (car->action == STRAIGHT) {
        int straightCount = getStraightCount(&light->base, car->position);
        if (straightCount == 0) {
            pthread_cond_broadcast(&light->no_straight);
        }
    }


	if (getLightState(&light->base) == EAST_WEST) {
        // Let waiting cars with East/West position know they are ready to go through
        pthread_cond_broadcast(&light->lightEW);
	}
    if (getLightState(&light->base) == NORTH_SOUTH) {
        // Let waiting cars with South/North position know they are ready to go through
        pthread_cond_broadcast(&light->lightSN);
	}
    pthread_mutex_unlock(&light->light_mutex);

	// Exit in same order as entering
	pthread_mutex_lock(&light->lanes[carLane].mutex);
    while(light->lanes[carLane].carsWaiting->index != car->index) {
        // Wait until car is front of the line
        pthread_cond_wait(&light->lanes[carLane].turn, &light->lanes[carLane].mutex);

    }

	exitIntersection(car, lane);

    // Update the waiting list and tell next car in the lane to go next
	CarOrder2 *carFree = light->lanes[carLane].carsWaiting ;
	light->lanes[carLane].carsWaiting = light->lanes[carLane].carsWaiting->next;
	free(carFree);
	pthread_cond_broadcast(&light->lanes[carLane].turn);
	pthread_mutex_unlock(&light->lanes[carLane].mutex);
}
