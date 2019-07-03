// Liam Wynn, 5/11/2018, Raycore

//#include <emscripten.h>
#include <stdio.h>

#include "loop_handling.h"

int main() {
	SDL_Window* window;
	SDL_Renderer* renderer;

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

	window = SDL_CreateWindow("S P A C E M A N", 0, 0, 320, 200, 0);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);

	//SDL_CreateWindowAndRenderer(320, 200, 0, &window, &renderer);

	// Initialize any non-SDL logic
	initialize(renderer);

	// Now run the loop.
	do_loop(renderer);

	// Actually sets the rendering loop.
	//emscripten_set_main_loop_arg(loop, &ctx, fps_count, run_infinite);

	clean_up();

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return EXIT_SUCCESS;
}
