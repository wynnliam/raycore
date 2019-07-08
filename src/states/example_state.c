// Liam Wynn, 7/8/2019, Raycore

#include "./example_state.h"
#include <stdio.h>


void state_example_initialize(SDL_Renderer* renderer) {
	printf("Example Initialize!\n");
}

void state_example_enter() {
	printf("Example Enter!\n");
}

void state_example_leave() {
	printf("Example Leave!\n");
}

void state_example_process_input() {
	printf("Example Process Input!\n");
}

void state_example_update() {
	printf("Example Update!\n");
}

void state_example_draw(SDL_Renderer* renderer) {
	printf("Example Draw!\n");
}

void state_example_clean_up() {
	printf("Example Clean Up!\n");
}

int state_example_quit() {
	return 0;
}
