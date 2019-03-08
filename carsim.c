/**
* CSC369 Assignment 2 - This file may be replaced when we run your submission.
* Don't put any custom code here needed for your submission!
*
* This is where the program is run from.
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "testing.h"

int main(int argc, char** argv)
{
    
	if (argc != 4) {
		fprintf(stderr, "Usage: %s [stop|light] number_of_experiments cars_per_experiment\n", argv[0]);
		exit(-1);
	}
	
	// Extract arguments.
	char* pEnd;
	const char* simulationName = argv[1];
	int experimentCount = strtol(argv[2], &pEnd, 10);
	int carsPerExperiment = strtol(argv[3], &pEnd, 10); 
	
	// Set the random seed using the current time.
	srand(time(NULL));
	
	// These are the experiments available. You can add your own for testing.
	typedef void(*SimFunction)(int);
	SimFunction sim = NULL;
	if (strcmp(simulationName, "stop") == 0) {
		sim = simulateStopSign;
	}
	if (strcmp(simulationName, "light") == 0) {
		sim = simulateTrafficLight;	
	}
	
	if (sim != NULL) {
		for (int i = 0; i < experimentCount; i++) {
			sim(carsPerExperiment);
		}
		printf("Simulation complete!\n"); 
	} else {
		fprintf(stderr, "Simulation not recognized: %s\n", simulationName);	
	}

}
