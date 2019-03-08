/**
* CSC369 Assignment 2 - This file may be replaced when we run your submission.
* Don't put any custom code here needed for your submission!
*
* Implementation of the various testing functions.
*/
#include "testing.h"
#include "common.h"
#include "car.h"
#include "safeStopSign.h"
#include "safeTrafficLight.h"

/**
* @brief Context object for a car-thread running in a simulation.
*
* This is passed in via pointer as the void* argument for the
* car-thread run function.
*/
typedef struct _CarContext {

	/**
	* @brief The car.
	*/
	Car car;

	/**
	* @brief The thread.
	*/
	pthread_t thread;

	/**
	* @brief Pointer to the stop sign if this is a stop sign scenario,
	* otherwise NULL.
	*/
	SafeStopSign* stopSign;

	/**
	* @brief Pointer to the traffic light if this is a traffic light scenario,
	* otherwise NULL.
	*/
	SafeTrafficLight* light;
} CarContext;

/**
* @brief How often to report that theads joined.
*/
const int THREAD_JOIN_REPORT_FREQUENCY = 15;

/**
* @brief Function to call for a car-thread.
*
* @param _context pointer to the context object.
*/
void* runCar(void* _context) {
	CarContext* context = (CarContext*)_context;
	if (context->stopSign != NULL) {
		runStopSignCar(&context->car, context->stopSign);
	} else if (context->light != NULL) {
		runTrafficLightCar(&context->car, context->light);
	} else {

		// This shouldn't happen.
		assert(FALSE);
	}
	
	return NULL;
}

/**
* @brief Starts a car moving.
*
* @param context pointer to the car-context.
* @param originalCopy pointer to the space where we can copy the car into
*   so that we can validate that it wasn't corrupted later on.
* @param index index of the context in the array of contexts.
* @param position the position the car will start from.
* @param action the action the car will perform.
* @param timeDelay delay in milliseconds to wait before the car starts up.
*/
void startCar(CarContext* context, Car* originalCopy, int index,
		CarPosition position, CarAction action, int timeDelay) {

	if (timeDelay > 0) {
		nap(timeDelay);
	}

	initCar(&context->car, index, position, action);
	*originalCopy = context->car;
	
	int result = pthread_create(&context->thread, NULL, runCar, context);
	if (result != 0){ 
		perror("Thread create failed."
				 "@ " __FILE__ " : " LINE_STRING "\n");	
	}
}

/**
* @brief Waits for all car-threads to join.
*
* @param contexts pointer to the array of car-contexts.
* @param carCount number of cars in the simulation.
*/
void joinAll(CarContext* contexts, int carCount) {

	// In the event of a deadlock, you'll see the main thread get stuck here.
	for (int i = 0; i < carCount; i++) {
		
		void* value_ptr;
		int result = pthread_join(contexts[i].thread, &value_ptr);
		if (result != 0) {
			perror("pthread_join failed "
				  "@ " __FILE__ " : " LINE_STRING "\n");	
		}
		
		if (i % THREAD_JOIN_REPORT_FREQUENCY == 0) {
			printf("  First %d threads have joined.\n", i + 1);
		}
	}

	printf("  All threads joined\n");
}

/**
* @brief Generates a scenario with a stop sign intersection.
*
* @param carCount number of cars in the scenario.
* @param contextsOut out-variable that will be set to a pointer to an array of
*   CarContexts containing one CarContext for each car in the simulation.
* @param originals out-variable that will be set to a pointer to an array of
*   Cars, which are copies of the ones being created so that we can detect
*   corruption in them.
* @return pointer to the SafeStopSign instance.
*/
SafeStopSign* generateStopSignScenario(int carCount, CarContext** contextsOut,
		Car** originalsOut) {

	// One reason not to allocate these arrays and structures on the stack
	// is that in C, threads are not allowed to access data from other 
	// threads' stacks.
	SafeStopSign* stopSign = (SafeStopSign*)malloc(sizeof(SafeStopSign));
	initSafeStopSign(stopSign, carCount);

	// Initialize the threads and related data.
	CarContext* contexts = (CarContext*)malloc(sizeof(CarContext) * carCount);
	for (int i = 0; i < carCount; i++) {
		contexts[i].stopSign = stopSign;
		contexts[i].light = NULL;
	}

	Car* originals = (Car*)malloc(sizeof(Car) * carCount);

	*contextsOut = contexts;
	*originalsOut = originals;

	return stopSign;
}


/**
* @brief Generates a traffic light scenario.
*
* @param horizontal number of cars starting from a horizontal direction.
* @param vertical number of cars starting from a vertical direction.
* @param contextsOut out-variable that will be set to a pointer to an array of
*   CarContexts containing one CarContext for each car in the simulation.
* @param originals out-variable that will be set to a pointer to an array of
*   Cars, which are copies of the ones being created so that we can detect
*   corruption in them.
*/
SafeTrafficLight* generateTrafficLightScenario(int horizontal, int vertical,
		CarContext** contextsOut, Car** originalsOut) {

	SafeTrafficLight* light = (SafeTrafficLight*)malloc(sizeof(SafeTrafficLight));
	initSafeTrafficLight(light, horizontal, vertical);

	// Initialize the threads and related data.
	int carCount = horizontal + vertical;
	CarContext* contexts = (CarContext*)malloc(sizeof(CarContext) * carCount);
	for (int i = 0; i < carCount; i++) {
		contexts[i].light = light;
		contexts[i].stopSign = NULL;
	}

	Car* originals = (Car*)malloc(sizeof(Car) * carCount);

	*contextsOut = contexts;
	*originalsOut = originals;
	return light;
}

/**
* @brief Helper for generating traffic light simulations that tries to use the
* given car starting position unless out of cars for that direction.
*
* With the traffic light, there are quotas on the number of cars moving
* horizontally and vertically in order to transition the light properly. The
* number of vertical and horizontal cars must be known in advance. This
* function helps generate cars that don't violate the quotas.
*
* @param hLeft number of cars left that travel horizontally. This is updated
*   based on the returned position.
* @param vLeft number of cars left that travel vertically. This is updated
*   based on the returned position.
*/
CarPosition tryPosition(CarPosition position, int* hLeft, int* vLeft) {
	assert(*hLeft > 0 || *vLeft > 0);
	assert(position == EAST || position == WEST || 
			position == SOUTH || position == NORTH);

	bool horizontal = position == EAST || position == WEST;
	if (horizontal && *hLeft == 0) {

		// Tried to make a horizontal starting position, but no horizontal cars
		// left to generate.
		(*vLeft)--;
		assert(*vLeft >= 0);
		return (CarPosition)((int)NORTH + 2 * (rand() % 2));
	} else if (!horizontal && *vLeft == 0) {

		// Tried to make a vertical starting position, but no vertical 
		// cars left to generate.
		(*hLeft)--;
		assert(*hLeft >= 0);
		return (CarPosition)((int)EAST + 2 * (rand() % 2));
	} else {

		// We have enough vertical or horizontal cars left that we can use the
		// original choice of starting position.
		if (horizontal) {
			(*hLeft)--;
			assert(*hLeft >= 0);
		} else {
			(*vLeft)--;
			assert(*vLeft >= 0);
		}
		return position;
	}
}

/**
* @brief Checks that two car objects match.
*
* Ignores the userPtr.
*
* @param a pointer to the first car.
* @param b pointer to the second car.
*/
bool checkCarMatch(Car* a, Car* b) {
	return a->action == b->action && a->position == b->position &&
		a->index == b->index;
}

/**
* @brief Confirms that the tokens assigned by something are all unique.
*
* @param tokens pointer to the array of tokens.
* @param tokenCount number of tokens in the set, including invalid ones.
*/
void checkTokensUnique(CarToken* tokenSet, int tokenCount) {

	// We will use a boolean array to check if a token has already been
	// assigned to another car.
	bool* seen = (bool*)malloc(sizeof(bool) * tokenCount);
	memset(seen, 0, sizeof(bool) * tokenCount);

	for (int i = 0; i < tokenCount; i++) {
		CarToken* token = &tokenSet[i];
		if (token->valid) {
			if (seen[token->token]) {
				printf("Token was assigned to more than one car.\n");
			}
			seen[token->token] = TRUE;
		}
	}

	free(seen);
}

/**
* @brief Checks various markers that indicate that the simulation worked as
* expected in the stop sign scenario.
*
* @param sign pointer to the stop sign.
* @param contexts pointer to the car-contexts.
* @param originals pointer to the copies of the cars as they originally were,
*   for detecting corruption.
* @param carCount number of cars in the simulation.
*/
void checkStopSign(SafeStopSign* sign, CarContext* contexts,
	Car* originals, int carCount) {

	printf("Checking token sets:\n");
	for (int i = 0; i < 4; i++) {
		checkTokensUnique(sign->base.entryLanes[i].enterTokens, carCount);
		checkTokensUnique(sign->base.entryLanes[i].exitTokens, carCount);
	}
	
	for (int i = 0; i < 4; i++) {
	    checkTokensUnique(sign->base.quadrants[i].tokens, carCount);
	}

	printf("Checking tokens for each car:\n");
	for (int i = 0; i < carCount; i++) {
		Car* original = &originals[i];

		// Check lane.
		{
			EntryLane* lane = getLane(original, &sign->base);
			if (!lane->enterTokens[i].valid) {
				fprintf(stderr, "Car %d did not enter lane.\n", i);
			}
			if (!lane->enterTokens[i].valid) {
				fprintf(stderr, "Car %d did not exit lane.\n", i);
			}

			// Check car matches in either case.
			if (!checkCarMatch(original, &lane->enterTokens[i].carCopy)) {
				fprintf(stderr, "Car %d does not match its entry lane copy.\n", i);
			}
			if (!checkCarMatch(original, &lane->exitTokens[i].carCopy)) {
				fprintf(stderr, "Car %d does not match its exit lane copy.\n", i);
			}
		}

		// Check stop sign tokens.
		int quadIndices[3];
		int quadCount = getStopSignRequiredQuadrants(original, quadIndices);
		for (int j = 0; j < quadCount; j++) {
		    IntersectionQuad* quad = &sign->base.quadrants[quadIndices[j]];
			if (!quad->tokens[i].valid) {
				fprintf(stderr, "Car %d did not get a stop sign token from "\
				    "one of the quadrants it was supposed to.\n", i);
			}
			if (!checkCarMatch(original, &quad->tokens[i].carCopy)) {
				fprintf(stderr, "Car %d does not match its stop sign copy.\n", i);
			}
		}
	}

	printf("\n");
}

/**
* @brief Checks various markers that indicate that the simulation worked as
* expected in the stop sign scenario.
*
* @param sign pointer to the stop sign.
* @param contexts pointer to the car-contexts.
* @param originals pointer to the copies of the cars as they originally were,
*   for detecting corruption.
* @param carCount number of cars in the simulation.
*/
void checkTrafficLight(SafeTrafficLight* light, CarContext* contexts,
	Car* originals, int carCount) {

	printf("Checking token sets:\n");
	for (int i = 0; i < TRAFFIC_LIGHT_LANE_COUNT; i++) {
		checkTokensUnique(light->base.entryLanes[i].enterTokens, carCount);
		checkTokensUnique(light->base.entryLanes[i].exitTokens, carCount);
	}
	checkTokensUnique(light->base.tokens, carCount);

	printf("Checking tokens for each car:\n");
	for (int i = 0; i < carCount; i++) {
		Car* original = &originals[i];

		// Check lane.
		{
			EntryLane* lane = getLaneLight(original, &light->base);
			if (!lane->enterTokens[i].valid) {
				fprintf(stderr, "Car %d did not enter lane.\n", i);
			}
			if (!lane->enterTokens[i].valid) {
				fprintf(stderr, "Car %d did not exit lane.\n", i);
			}

			// Check car matches in either case.
			if (!checkCarMatch(original, &lane->enterTokens[i].carCopy)) {
				fprintf(stderr, "Car %d does not match its entry lane copy.\n", i);
			}
			if (!checkCarMatch(original, &lane->exitTokens[i].carCopy)) {
				fprintf(stderr, "Car %d does not match its exit lane copy.\n", i);
			}
		}

		// Check traffic light tokens.
		{
			if (!light->base.tokens[i].valid) {
				fprintf(stderr, "Car %d did not get a traffic light token.\n", i);
			}
			if (!checkCarMatch(original, &light->base.tokens[i].carCopy)) {
				fprintf(stderr, "Car %d does not match its traffic light copy.\n", i);
			}
		}
	}

	printf("\n");
}

void simulateStopSign(int carCount) {
	printf("Simulate Stop Sign: %d\n", carCount);

	// Do some standard set-up.
	CarContext* contexts;
	Car* originals;
	SafeStopSign* sign = generateStopSignScenario(carCount, &contexts,
		&originals);

	// Run the simulation and wait for threads to join.
	for (int i = 0; i < carCount; i++) {
		int timeDelay = maxA2(rand() % 3000 - 600, 0);
		CarPosition pos = (CarPosition)(rand() % 4);
		CarAction action = (CarAction)(rand() % 3);
		startCar(&contexts[i], &originals[i], i, pos, action, timeDelay);
	}
	joinAll(contexts, carCount);

	// Validate that the simulation proceeded correctly.
	checkStopSign(sign, contexts, originals, carCount);

	// Delete everything we allocated.
	destroySafeStopSign(sign);
	free(sign);
	free(contexts);
	free(originals);
}

void simulateTrafficLight(int carCount) {
	printf("Simulate Traffic Light: %d\n", carCount);

	CarContext* contexts;
	Car* originals;
	int horizontal = rand() % carCount;
	int vertical = carCount - horizontal;

	SafeTrafficLight* light = generateTrafficLightScenario(horizontal,
			vertical, &contexts, &originals);	

	// Run the simulation. We have to be careful to generate the right number
	// of horizontal and vertical cars.
	int hLeft = horizontal;
	int vLeft = vertical;
	for (int i = 0; i < carCount; i++) {

		int timeDelay = 0;
		if (rand() % 10 == 1) {
			timeDelay = rand() % 2000;
		}
		CarPosition pos = tryPosition((CarPosition)(rand() % 4), &hLeft, &vLeft);
		CarAction action = (CarAction)(rand() % 3);
		startCar(&contexts[i], &originals[i], i, pos, action, timeDelay);
	}

	// Wait for threads to join and then confirm results are good.
	joinAll(contexts, carCount);
	checkTrafficLight(light, contexts, originals, carCount);

	// Delete everything.
	destroySafeTrafficLight(light);
	free(light);
	free(contexts);
	free(originals);
}
