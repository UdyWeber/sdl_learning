#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_log.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <optional>
#include <stdio.h>
#include <vector>

#define SW 640
#define SH 480
#define CELL_DIM 10
#define SCREEN_FPS 30
#define SCREEN_TICK_PER_FRAME (1000 / SCREEN_FPS)
#define GRID_SIZE ((SW / CELL_DIM) * (SH / CELL_DIM) - 1)

SDL_Window *win = NULL;
SDL_Renderer *rend = NULL;

class Grid {
private:
  size_t rows;
  size_t columns;
  std::vector<std::optional<SDL_Rect>> points;

  bool exists(int x, int y);
  bool can_move(SDL_Rect rect);
  void create_random_point();
  void spawn_at(int x, int y);
  size_t into_grid_index(int x, int y);

public:
  Grid(size_t r, size_t c, int pts);
  void update();
  void render();
};

size_t Grid::into_grid_index(int x, int y) { return x * columns + y; }

bool Grid::exists(int x, int y) {
  return points[into_grid_index(x, y)].has_value();
}

bool Grid::can_move(SDL_Rect rect) { return rect.y < SH - CELL_DIM; }

void Grid::spawn_at(int idx_x, int idx_y) {
  SDL_Log("Spawining CELL {X: %d, Y: %d}", idx_x, idx_y);
  points[into_grid_index(idx_x, idx_y)] = {
      .x = idx_x * CELL_DIM,
      .y = idx_y * CELL_DIM,
      .w = CELL_DIM,
      .h = CELL_DIM,
  };
}

void Grid::create_random_point() {
  int cell_y = std::floor((rand() % (SH / 3)) / CELL_DIM);
  int cell_x = std::floor((rand() % SW) / CELL_DIM);

  while (exists(cell_x, cell_y)) {
    cell_y = std::floor((rand() % (SH / 3)) / CELL_DIM);
    cell_x = std::floor((rand() % SW) / CELL_DIM);
  }

  spawn_at(cell_x, cell_y);
};

Grid::Grid(size_t r, size_t c, int pts = 10)
    : rows(r), columns(c), points(r * c) {
  std::srand(std::time(0));

  for (int i = 0; i < pts; i++) {
    create_random_point();
  }
}

void Grid::render() {
  SDL_SetRenderDrawColor(rend, 0xFF, 0xFF, 0xFF, 0xFF);
  SDL_RenderClear(rend);

  SDL_SetRenderDrawColor(rend, 240, 20, 19, 0);
  for (auto p : points) {
    if (!p.has_value())
      continue;
    SDL_RenderFillRect(rend, &p.value());
  }
  SDL_RenderPresent(rend);
}

void Grid::update() {
  for (auto &p : points) {
    if (!p.has_value() || !can_move(p.value()))
      continue;

    p->y += CELL_DIM;
  }
}

bool init() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("Could not initialize SDL! ERROR: %s\n", SDL_GetError());
    return false;
  }

  win = SDL_CreateWindow("Sand Spiller", SDL_WINDOWPOS_UNDEFINED,
                         SDL_WINDOWPOS_UNDEFINED, SW, SH, SDL_WINDOW_SHOWN);

  if (win == NULL) {
    return false;
  }

  rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

  if (rend == NULL) {
    return false;
  }

  return true;
}

void close() {
  SDL_DestroyWindow(win);
  SDL_DestroyRenderer(rend);

  win = NULL;
  rend = NULL;

  SDL_Quit();
}

int main() {
  if (!init()) {
    return -1;
  }

  SDL_Event e;
  bool quit = false;

  auto grid = new Grid(SH / CELL_DIM, SW / CELL_DIM);

  auto previous = SDL_GetTicks();
  Uint32 lag = 0;

  while (!quit) {
    auto current = SDL_GetTicks();
    auto elapsed = current - previous;
    previous = current;
    lag += elapsed;

    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
        quit = true;
      }
    }

    while (lag >= SCREEN_TICK_PER_FRAME) {
      grid->update();
      lag -= SCREEN_TICK_PER_FRAME;
    }

    grid->render();

    Uint32 frameTime = SDL_GetTicks() - current;
    if (frameTime < SCREEN_TICK_PER_FRAME) {
      SDL_Delay(SCREEN_TICK_PER_FRAME - frameTime);
    }
  }

  close();
  return 0;
}
