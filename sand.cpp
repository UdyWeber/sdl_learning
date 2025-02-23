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
#include <stdio.h>
#include <tuple>
#include <vector>

#define SW 640
#define SH 480
#define CELL_DIM 10
#define SCREEN_TICK_PER_FRAME 100
#define GRID_SIZE ((SW / CELL_DIM) * (SH / CELL_DIM) - 1)

SDL_Window *win = NULL;
SDL_Renderer *rend = NULL;

class Grid {
private:
  int rows;
  int columns;
  std::vector<int> points;

  bool exists(int x, int y);
  bool can_move(int idx);
  void create_random_point();
  void spawn_at(int x, int y);
  SDL_Rect create_rect(int x, int y);
  size_t into_grid_index(int x, int y);
  std::tuple<int, int> coords(int idx);

public:
  Grid(size_t r, size_t c, int pts);
  void update();
  void render();
};

SDL_Rect Grid::create_rect(int x, int y) {
  return {
      .x = x * CELL_DIM,
      .y = y * CELL_DIM,
      .w = CELL_DIM,
      .h = CELL_DIM,
  };
};

std::tuple<int, int> Grid::coords(int idx) {
  return std::make_tuple(idx % columns, idx / columns);
}
size_t Grid::into_grid_index(int x, int y) { return x * columns + y; }

bool Grid::exists(int x, int y) { return points[into_grid_index(x, y)] != 0; }

bool Grid::can_move(int idx) {
  auto [_, y] = coords(idx);
  return y < rows - 1 && points[idx + columns] == 0;
}

void Grid::spawn_at(int idx_x, int idx_y) {
  SDL_Log("Spawining CELL {X: %d, Y: %d}", idx_x, idx_y);
  points[into_grid_index(idx_x, idx_y)] = 1;
}

void Grid::create_random_point() {
  int cell_y = std::floor((rand() % SW) / CELL_DIM);
  int cell_x = std::floor((rand() % SH) / CELL_DIM);

  while (exists(cell_x, cell_y)) {
    cell_y = std::floor((rand() % SW) / CELL_DIM);
    cell_x = std::floor((rand() % SH) / CELL_DIM);
  }

  spawn_at(cell_x, cell_y);
};

Grid::Grid(size_t r, size_t c, int pts = 1000)
    : rows(r), columns(c), points(r * c) {
  std::srand(std::time(0));

  for (int i = 0; i < pts; i++) {
    create_random_point();
  }
}

void Grid::render() {
  SDL_SetRenderDrawColor(rend, 0xFF, 0xFF, 0xFF, 0xFF);
  SDL_RenderClear(rend);

  for (int i = 0; i < points.size(); i++) {
    if (points[i] == 0)
      continue;

    auto [x, y] = coords(i);
    const auto r = create_rect(x, y);

    SDL_SetRenderDrawColor(rend, 240, 20, 19, 0);
    SDL_RenderFillRect(rend, &r);
  }

  SDL_RenderPresent(rend);
}

void Grid::update() {
  for (int i = 0; i < points.size(); i++) {
    const auto p = points[i];

    if (p == 0 || !can_move(i)) {
      points[i] = p;
      continue;
    }

    points[i] = 0;
    points[i + columns] = p;
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
