#include "car.h"
/**
* CSC369 Assignment 2 - This file may be replaced when we run your code.
*
* Implementation of the Car.
*/

CarPosition getOppositePosition(CarPosition position) {
	return (CarPosition)((position + 2) % 4);
}

void initCar(Car* car, int index, CarPosition position, CarAction action) {
	car->index = index;
	car->position = position;
	car->action = action;
	car->userPtr = NULL;
}