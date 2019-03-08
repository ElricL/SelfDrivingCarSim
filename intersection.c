/**
* CSC369 Assignment 2 - Don't modify this file!
*
* This is the implementation for intersection logic.
*/
#include "intersection.h"
#include "common.h"
#include "car.h"

void initToken(CarToken* carToken, Car* car, int tokenValue) {
	assert(!carToken->valid);

	carToken->carCopy = *car;
	carToken->token = tokenValue;
	carToken->valid = TRUE;
}

void enterLane(Car* car, EntryLane* lane) {
	int token = lane->enterCounter;

	int duration = (rand() % 750) + 250;
	nap(duration);
	lane->enterCounter = token + 1;

	initToken(&lane->enterTokens[car->index], car, token);
}

void exitIntersection(Car* car, EntryLane* lane) {
	int duration = (rand() % 500) - 250;
	nap(duration);

	if (!lane->enterTokens[car->index].valid) {
		fprintf(stderr, "Car did not enter this lane "\
				"@ " __FILE__ " : " LINE_STRING "\n");
	}

	// Car must exit in the same order it entered.
	int enterToken = lane->enterTokens[car->index].token;
	int exitToken = lane->exitCounter++;
	if (enterToken != exitToken) {

		fprintf(stderr, "Car did not exit in the same order as it entered "\
				"@ " __FILE__ " : " LINE_STRING "\n");
	} else {
		initToken(&lane->exitTokens[car->index], car, exitToken);
	}
}
