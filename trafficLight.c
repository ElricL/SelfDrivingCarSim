/**
* CSC369 Assignment 2 - This file may be replaced when we run your code.
*
* This contains the implementation of the (unsafe) traffic light.
*/
#include "trafficLight.h"
#include "common.h"

void initTrafficLight(TrafficLight* light, int eastWest, int northSouth) {

	// Initialize lanes.
	int carCount = eastWest + northSouth;
	for (int i = 0; i < TRAFFIC_LIGHT_LANE_COUNT; i++) {
		size_t size = sizeof(CarToken) * carCount;
		light->entryLanes[i].enterTokens = (CarToken*)malloc(size);
		memset(light->entryLanes[i].enterTokens, 0, size);
		light->entryLanes[i].exitTokens = (CarToken*)malloc(size);
		memset(light->entryLanes[i].exitTokens, 0, size);

		light->entryLanes[i].enterCounter = 0;
		light->entryLanes[i].exitCounter = 0;
	}

	initMutex(&light->validationLock);
	
	// Since the light transitions based on the number of cars that pass 
	// through it, a deadlock will happen if the light expects more 
	// east-west or more north-south cars to pass than are actually left in
	// the simulation.
	light->totalEWLeft = eastWest;
	light->totalNSLeft = northSouth;

	// Initialize tokens for the intersection.
	{
		size_t size = sizeof(CarToken) * carCount;
		light->tokens = (CarToken*)malloc(size);
		memset(light->tokens, 0, size);

		light->tokenCounter = 0;
	}

	// Initialize the allowed direction of the light and its duration.
	light->carsInside = 0;
	if (light->totalEWLeft > 0) {
		light->currentMode = EAST_WEST;
		light->carsLeft = minA2(light->totalEWLeft, 10);
	} else if (light->totalNSLeft > 0) {
		light->currentMode = NORTH_SOUTH;
		light->carsLeft = minA2(light->totalNSLeft, 10);
	} else {

		// Should be at least one car!
		assert(FALSE);
	}
	
	for (int i = 0; i < DIRECTION_COUNT; i++) {
		light->straightCounts[i] = 0;	
	}
}

void destroyTrafficLight(TrafficLight* light) {

	// Free lanes.
	for (int i = 0; i < DIRECTION_COUNT; i++) {
		free(light->entryLanes[i].enterTokens);
		free(light->entryLanes[i].exitTokens);
	}

	pthread_mutex_destroy(&light->validationLock);
	
	free(light->tokens);
}

int getLaneIndexLight(Car* car) {
	return car->position * 3 + car->action;
}

EntryLane* getLaneLight(Car* car, TrafficLight* intersection) {
	int laneIndex = getLaneIndexLight(car);
	return &intersection->entryLanes[laneIndex];
}

void enterTrafficLight(Car* car, TrafficLight* intersection) {

	// Validate and set internal counters.
	{
		pthread_mutex_lock(&intersection->validationLock);

		// Validate that the light is green for the car.
		if ((car->position == EAST || car->position == WEST) &&
				intersection->currentMode != EAST_WEST) {
			fprintf(stderr, "Car from east or west attempted to enter intersection "\
				"not in east-west mode."\
				"@ " __FILE__ " : " LINE_STRING "\n");

			// Important that we release the lock before we return.
			unlock(&intersection->validationLock);
			return;
		} else if ((car->position == NORTH || car->position == SOUTH) &&
				intersection->currentMode != NORTH_SOUTH) {
			fprintf(stderr, "Car from north or south attempted to enter intersection "\
					"not in north-south mode."\
					"@ " __FILE__ " : " LINE_STRING "\n");
			unlock(&intersection->validationLock);
			return;
		}

		if (car->action == STRAIGHT) {
			intersection->straightCounts[(int)car->position]++;
		}

		unlock(&intersection->validationLock);
	}

	// Update number of cars left from whichever orientation.
	{
		if (car->position == EAST || car->position == WEST) {
			intersection->totalEWLeft--;
			assert(intersection->totalEWLeft >= 0);
		} else if (car->position == NORTH || car->position == SOUTH) {
			intersection->totalNSLeft--;
			assert(intersection->totalNSLeft >= 0);
		} else {

			// Shouldn't go here.
			assert(FALSE);
		}
	}

	// carsLeft represents the amount of "time" left before the light
	// transitions to Red. In practice, this is the number of cars that can
	// enter the intersection before it turns red. Once it hits 0, the light
	// transitions to Red.
	intersection->carsLeft--;
	assert(intersection->carsLeft >= 0);
	if (intersection->carsLeft == 0) {
		assert(intersection->currentMode != RED);
		intersection->previousDirection = intersection->currentMode;
		intersection->currentMode = RED;
	}
	intersection->carsInside++;
}

void actTrafficLight(Car* car, TrafficLight* intersection,
		IntersectionCallback beforeSleep, IntersectionCallback afterSleep,
		void* userPtr) {

	// Validate and set internal counters.
	{
		pthread_mutex_lock(&intersection->validationLock);

		// Check for collisions. To make things easier, only left-turners need to
		// to confirm there are no cars going straight from the opposite direction.
		// Cars going straight can ignore left-turners.
		if (car->action == LEFT_TURN) {

			CarPosition opposite = getOppositePosition(car->position);
			int straightCount = getStraightCount(intersection, opposite);
			if (straightCount > 0) {
				fprintf(stderr, "Car attempting to left turn while straight traffic "\
						"from the opposite direction is coming. Collision! "\
						"@ " __FILE__ " : " LINE_STRING "\n");
				unlock(&intersection->validationLock);
				return;
			}
		}

		unlock(&intersection->validationLock);
	}

	// Call user before-sleep callback.
	if (beforeSleep != NULL) {
		beforeSleep(userPtr);
	}

	// Sleep. Duration depends on the action.
	int durations[3];
	durations[STRAIGHT] = 2;
	durations[RIGHT_TURN] = 1;
	durations[LEFT_TURN] = 3;
	int duration = durations[car->action];
	nap(duration * 1000);

	// Call user after-sleep callback.
	if (afterSleep != NULL) {
		afterSleep(userPtr);
	}

	// Give the car a token for navigating the intersection.
	EntryLane* lane = getLaneLight(car, intersection);
	if (!lane->enterTokens[car->index].valid || lane->exitTokens[car->index].valid) {
		fprintf(stderr, "Car either has not entered a lane or has already exited. "\
				"@ " __FILE__ " : " LINE_STRING "\n");
	}
	int token = intersection->tokenCounter++;
	initToken(&intersection->tokens[car->index], car, token);

	// Cars inside represents the amount of "time" left before the
	// light transitions to Green from Red. In practice, this is the number
	// of cars still in the intersection and that need to clear it before
	// the traffic light lets the next direction go.
	intersection->carsInside--;
	assert(intersection->carsInside >= 0);
	
	// When this is red (for both sides) and there are no cars inside, we
	// transition back to green for one of the directions.
	if (intersection->carsInside == 0 && intersection->currentMode == RED) {

		LightState oldMode = intersection->previousDirection;
		intersection->currentMode = getOppositeDirection(oldMode);

		// Use of min prevents deadlock when there are only a low number of
		// cars left in the new direction. At that point, we want to prevent
		// waiting for more cars than actually exist to go.
		int directionCarsLeft = intersection->currentMode == EAST_WEST ?
				intersection->totalEWLeft :
				intersection->totalNSLeft;
		
		// Turn on again for the old direction if no cars are waiting for 
		// the new one.
		if (directionCarsLeft == 0) {
			intersection->currentMode = getOppositeDirection(
					intersection->currentMode);
			directionCarsLeft = intersection->currentMode == EAST_WEST ?
					intersection->totalEWLeft :
					intersection->totalNSLeft;
		}

		int randomLightAmount = rand() % 5 + 1;
		intersection->carsLeft = minA2(directionCarsLeft, randomLightAmount);
	}

	// Update straight counts appropriately.
	if (car->action == STRAIGHT) {
		pthread_mutex_lock(&intersection->validationLock);
		intersection->straightCounts[(int)car->position]--;
		unlock(&intersection->validationLock);
	}

}

LightState getOppositeDirection(LightState mode) {
	assert(mode != RED);
	return (LightState)(1 - (int)mode);
}

LightState getLightState(TrafficLight* light) {
	return light->currentMode;
}

int getStraightCount(TrafficLight* light, int position) {
	return light->straightCounts[position];
}
