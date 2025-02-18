#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>
#include <string>
#include <vector>

#define SCREEN_HEIGHT 480
#define SCREEN_WIDTH 640

enum KeyPressedTexture {
  DEFAULT_PRESS,
  UP_PRESS,
  DOWN_PRESS,
  LEFT_PRESS,
  RIGHT_PRESS,
  KEY_PRESS_TOTAL
};

SDL_Window *g_window = NULL;
SDL_Renderer *g_screen_rendered = NULL;
SDL_Texture *g_current_texture = NULL;
SDL_Texture *g_textures[KEY_PRESS_TOTAL];
std::vector<std::string> files = {"meme.bmp", "meme2.bmp", "meme3.bmp",
                                  "meme4.bmp", "meme5.bmp"};

bool init() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("Couldnt init SDL! SDL_Error: %s\n", SDL_GetError());
    return false;
  }

  g_window = SDL_CreateWindow("Key Press Example", SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                              SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

  if (g_window == nullptr) {
    printf("Not able to start window! SDL_Error: %s\n", SDL_GetError());
    return false;
  }

  g_screen_rendered =
      SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED);

  if (g_screen_rendered == nullptr) {
    printf("Not able to start renderer! SDL_Error: %s\n", SDL_GetError());
    return false;
  }

  SDL_SetRenderDrawColor(g_screen_rendered, 0xFF, 0xFF, 0xFF, 0xFF);

  return true;
}

SDL_Texture *load_texture(std::string path) {
  SDL_Texture *texture = nullptr;
  auto surface = SDL_LoadBMP(path.c_str());

  if (surface == nullptr) {
    printf("Failed to load BMP! SDL_Error: %s\n", SDL_GetError());
    return texture;
  }

  texture = SDL_CreateTextureFromSurface(g_screen_rendered, surface);

  if (texture == nullptr) {
    printf("Failed to load texture from BMP! SDL_Error: %s\n", SDL_GetError());
    return texture;
  }

  SDL_FreeSurface(surface);

  return texture;
}

bool load_media() {
  int idx = 0;
  for (const std::string image : files) {
    auto t = load_texture("files/" + image);
    if (t == nullptr) {
      printf("Error occurred while loading texture! SDL_Error: %s\n",
             SDL_GetError());
      return false;
    }
    g_textures[idx] = t;
    idx++;
  }
  return true;
}
void finish() {
  for (int i = 0; i < KEY_PRESS_TOTAL; i++) {
    auto t = g_textures[i];
    if (t != nullptr) {
      SDL_DestroyTexture(t);
      g_textures[i] = nullptr;
    }
  }

  SDL_DestroyWindow(g_window);
  SDL_DestroyRenderer(g_screen_rendered);
  SDL_DestroyTexture(g_current_texture);

  g_window = NULL;
  g_screen_rendered = NULL;
  g_current_texture = NULL;

  SDL_Quit();
}

int main() {
  if (!init()) {
    printf("Couldnt start SDL...");
    return -1;
  }

  if (!load_media()) {
    printf("Error while loading media...");
    return -1;
  }

  bool quit = false;
  SDL_Event e;

  while (!quit) {

    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
        quit = true;
      }

      switch (e.key.keysym.sym) {
      case SDLK_UP:
        g_current_texture = g_textures[UP_PRESS];
        break;
      case SDLK_DOWN:
        g_current_texture = g_textures[DOWN_PRESS];
        break;
      case SDLK_LEFT:
        g_current_texture = g_textures[LEFT_PRESS];
        break;
      case SDLK_RIGHT:
        g_current_texture = g_textures[RIGHT_PRESS];
        break;
      default:
        g_current_texture = g_textures[DEFAULT_PRESS];
        break;
      }

      SDL_RenderClear(g_screen_rendered);
      SDL_RenderCopy(g_screen_rendered, g_current_texture, NULL, NULL);
      SDL_RenderPresent(g_screen_rendered);
    }
  }

  finish();
  return 0;
}
