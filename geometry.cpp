#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <cstddef>
#include <stdio.h>

#define SCREEN_W 640
#define SCREEN_H 480

SDL_Window *g_window = NULL;
SDL_Renderer *g_renderer = NULL;

bool init() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("Failed to run SDL! SDL_Error: %s\n", SDL_GetError());
    return false;
  }

  g_window = SDL_CreateWindow("Geometry exmaple", SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED, SCREEN_W, SCREEN_H,
                              SDL_WINDOW_SHOWN);

  if (g_window == NULL) {
    return false;
  }

  g_renderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED);

  if (g_renderer == NULL) {
    return false;
  }

  return true;
}

void finish() {
  SDL_DestroyWindow(g_window);
  SDL_DestroyRenderer(g_renderer);

  g_window = NULL;
  g_renderer = NULL;
}

int main(int argc, char *args[]) {
  if (!init()) {
    printf("Failed to init");
    return -1;
  }

  bool quit = false;
  SDL_Event e;

  while (!quit) {
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
        quit = true;
      }
    }

    SDL_SetRenderDrawColor(g_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(g_renderer);

    SDL_Rect rect = {SCREEN_W / 4, SCREEN_H / 4, SCREEN_W / 2, SCREEN_H / 2};
    SDL_SetRenderDrawColor(g_renderer, 0xFF, 0x00, 0x00, 0xFF);
    SDL_RenderFillRect(g_renderer, &rect);

    SDL_Rect outlineRect = {SCREEN_W / 6, SCREEN_H / 6, SCREEN_W * 2 / 3,
                            SCREEN_H * 2 / 3};
    SDL_SetRenderDrawColor(g_renderer, 0x00, 0xFF, 0x00, 0xFF);
    SDL_RenderDrawRect(g_renderer, &outlineRect);

    SDL_SetRenderDrawColor(g_renderer, 0x00, 0x00, 0xFF, 0xFF);
    SDL_RenderDrawLine(g_renderer, 0, SCREEN_H / 2, SCREEN_W, SCREEN_H / 2);

    SDL_SetRenderDrawColor(g_renderer, 0xFF, 0xFF, 0x00, 0xFF);
    for (int i = 0; i < SCREEN_H; i += 4) {
      SDL_RenderDrawPoint(g_renderer, SCREEN_W / 2, i);
    }

    SDL_RenderPresent(g_renderer);
  }

  finish();
  return 0;
}
