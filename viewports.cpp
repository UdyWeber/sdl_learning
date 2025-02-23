
#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <cstddef>
#include <stdio.h>
#include <string>

#define SCREEN_W 640
#define SCREEN_H 480

SDL_Window *g_window = NULL;
SDL_Renderer *g_renderer = NULL;

bool init() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("Failed to run SDL! SDL_Error: %s\n", SDL_GetError());
    return false;
  }

  g_window = SDL_CreateWindow("Viewport example", SDL_WINDOWPOS_UNDEFINED,
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

  SDL_Quit();
}

SDL_Texture *load_texture(std::string path) {
  SDL_Texture *texture = nullptr;
  auto surface = SDL_LoadBMP(path.c_str());

  if (surface == nullptr) {
    printf("Failed to load BMP! SDL_Error: %s\n", SDL_GetError());
    return texture;
  }

  texture = SDL_CreateTextureFromSurface(g_renderer, surface);

  if (texture == nullptr) {
    printf("Failed to load texture from BMP! SDL_Error: %s\n", SDL_GetError());
    return texture;
  }

  SDL_FreeSurface(surface);

  return texture;
}

int main(int argc, char *args[]) {
  if (!init()) {
    printf("Failed to init");
    return -1;
  }

  bool quit = false;
  SDL_Event e;

  auto t = load_texture("files/meme.bmp");
  auto t2 = load_texture("files/meme2.bmp");
  auto t4 = load_texture("files/meme4.bmp");

  while (!quit) {
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
        quit = true;
      }
    }

    SDL_Rect tl_view = {
        .x = 0,
        .y = 0,
        .w = SCREEN_W / 2,
        .h = SCREEN_H / 2,
    };
    SDL_RenderCopy(g_renderer, t, NULL, &tl_view);

    SDL_Rect tr_view = {
        .x = SCREEN_W / 2,
        .y = 0,
        .w = SCREEN_W / 2,
        .h = SCREEN_H / 2,
    };
    SDL_RenderCopy(g_renderer, t2, NULL, &tr_view);

    SDL_Rect b_view = {
        .x = 0,
        .y = SCREEN_H / 2,
        .w = SCREEN_W,
        .h = SCREEN_H / 2,
    };
    SDL_RenderCopy(g_renderer, t4, NULL, &b_view);

    // INFO: Each SDL_RenderCopy will copy to the setted viewport so if I render
    // copy something to the renderer while a view port is being set it will
    // copy to the specific rect
    SDL_RenderPresent(g_renderer);
  }

  SDL_DestroyTexture(t);
  SDL_DestroyTexture(t2);
  SDL_DestroyTexture(t4);

  finish();
  return 0;
}
