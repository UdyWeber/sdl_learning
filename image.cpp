#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <cstddef>
#include <stdio.h>

SDL_Window *g_window = NULL;
SDL_Surface *g_screen_surface = NULL;
SDL_Surface *g_hello_world = NULL;
SDL_Renderer *g_renderer = NULL;
SDL_Texture *g_uga_texture = NULL;

const int SCREEN_HEIGHT = 480;
const int SCREEN_WIDTH = 640;

bool init() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    return false;
  }

  if (!(IMG_Init(IMG_INIT_JPG) & IMG_INIT_JPG)) {
    printf("SDL Image lib not initialized! IMG_ErrorÇ %s\n", IMG_GetError());
    return false;
  }

  g_window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                              SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

  if (g_window == NULL) {
    printf("SDL failed to create window! SDL_Error: %s\n", SDL_GetError());
    return false;
  }

  g_screen_surface = SDL_GetWindowSurface(g_window);

  g_renderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED);
  if (g_renderer == NULL) {
    printf("SDL failed creating renderer for image... SDL Error: %s\n",
           SDL_GetError());
    return false;
  }

  return true;
}

bool load_media() {
  g_hello_world = IMG_Load("files/uga.jpg");
  if (g_hello_world == NULL) {
    printf("SDL failed to load god's image on earth :c SDL_Error: %s\n",
           SDL_GetError());
    return false;
  }

  g_uga_texture = SDL_CreateTextureFromSurface(g_renderer, g_hello_world);
  if (g_uga_texture == NULL) {
    printf("Couldnt load surface into texture :c SDL_Error: %s\n",
           SDL_GetError());
    return false;
  }

  return true;
}

void finish() {
  SDL_FreeSurface(g_hello_world);
  g_hello_world = NULL;

  SDL_DestroyWindow(g_window);
  g_window = NULL;

  SDL_DestroyTexture(g_uga_texture);
  g_uga_texture = NULL;

  SDL_DestroyRenderer(g_renderer);
  g_renderer = NULL;

  IMG_Quit();
  SDL_Quit();
}

int main(int argc, char *args[]) {
  if (!init()) {
    printf("Failed initilizing SDL internals...");
    return -1;
  }
  if (!load_media()) {
    printf("Failed loading image to SDL...");
    return -1;
  }

  SDL_Event e;
  bool quit = false;
  while (!quit) {
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT)
        quit = true;
    }
    SDL_RenderClear(g_renderer);
    SDL_RenderCopy(g_renderer, g_uga_texture, NULL, NULL);
    SDL_RenderPresent(g_renderer);
    SDL_Delay(16);
  }

  finish();
}
