#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_mutex.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

int main( int argc, char* args[] ) {	
	SDL_Window* window = NULL;
	SDL_Surface* screen_surface = NULL;

	if (SDL_Init( SDL_INIT_VIDEO ) < 0) {
		printf("SDL could not initialize SDL_Error: %s\n", SDL_GetError());
		return -1;
	}
	
	window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (window == NULL) {
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
	}

	screen_surface = SDL_GetWindowSurface( window );
	SDL_FillRect(screen_surface, NULL, SDL_MapRGB( screen_surface->format, 0xFF, 0xFF, 0xFF));

	SDL_UpdateWindowSurface(window);
	
	SDL_Event e; bool quit = false;
	while( quit == false) {
		while(SDL_PollEvent(&e)) {
			if ( e.type == SDL_QUIT ) quit = true;
		}
	}

	SDL_DestroyWindow( window );
	SDL_Quit();

	return 0;
}
