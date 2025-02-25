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

constexpr int SW = 1280;
constexpr int SH = 720;
constexpr int CELL_DIM = 2;
constexpr int SCREEN_TICK_PER_FRAME = 30;
constexpr int DEFAULT_INITIAL_CELLS = 0;
constexpr int GRID_SIZE = ((SW / CELL_DIM) * (SH / CELL_DIM) - 1);
constexpr int DROP_SIZE = 100;

SDL_Window *win = NULL;
SDL_Renderer *rend = NULL;

// INFO: Just copied to see if it works :D
void HSVtoRGB(float h, float s, float v, Uint8 &r, Uint8 &g, Uint8 &b) {
  float c = v * s;
  float x = c * (1 - fabs(fmod(h / 60.0, 2) - 1));
  float m = v - c;

  float r1, g1, b1;
  if (h < 60) {
    r1 = c, g1 = x, b1 = 0;
  } else if (h < 120) {
    r1 = x, g1 = c, b1 = 0;
  } else if (h < 180) {
    r1 = 0, g1 = c, b1 = x;
  } else if (h < 240) {
    r1 = 0, g1 = x, b1 = c;
  } else if (h < 300) {
    r1 = x, g1 = 0, b1 = c;
  } else {
    r1 = c, g1 = 0, b1 = x;
  }

  r = (r1 + m) * 255;
  g = (g1 + m) * 255;
  b = (b1 + m) * 255;
}

class Grid {
private:
  int rows;
  int columns;
  std::vector<int> points;

  bool exists(int x, int y);
  bool can_move(int idx);
  void create_random_point();
  void spawn_at(int x, int y, int color);
  SDL_Rect create_rect(int x, int y);
  size_t into_grid_index(int x, int y);
  std::tuple<int, int> coords(int idx);
  std::tuple<int, int> window_to_grid_coords(int x, int y);

public:
  Grid(size_t r, size_t c, int pts);
  void update();
  void render();
  void handle_mouse(int hue);
  void clean();
};

void Grid::clean() { points.clear(); }

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

std::tuple<int, int> Grid::window_to_grid_coords(int x, int y) {
  int cell_x = std::floor(x / CELL_DIM);
  int cell_y = std::floor(y / CELL_DIM);
  return std::make_tuple(cell_x, cell_y);
}

size_t Grid::into_grid_index(int x, int y) { return y * columns + x; }

bool Grid::exists(int x, int y) { return points[into_grid_index(x, y)] != 0; }

bool Grid::can_move(int idx) {
  auto [_, y] = coords(idx);
  return y < rows - 1 && points[idx + columns] == 0;
}

void Grid::spawn_at(int grid_x, int grid_y, int color) {
  if (exists(grid_x, grid_y))
    return;
  points[into_grid_index(grid_x, grid_y)] = color;
}

void Grid::create_random_point() {
  auto [cell_x, cell_y] = window_to_grid_coords(rand() % SH, rand() % SW);
  while (exists(cell_x, cell_y)) {
    std::tie(cell_x, cell_y) = window_to_grid_coords(rand() % SH, rand() % SW);
  }
  spawn_at(cell_x, cell_y, (rand() * 69 * 420) % 360);
};

Grid::Grid(size_t r, size_t c, int pts = DEFAULT_INITIAL_CELLS)
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
    const auto rect = create_rect(x, y);

    Uint8 r, g, b;
    HSVtoRGB(points[i], 1.0, 1.0, r, g, b);

    SDL_SetRenderDrawColor(rend, r, g, b, 255);
    SDL_RenderFillRect(rend, &rect);
  }

  SDL_RenderPresent(rend);
}

void Grid::update() {
  auto next_grid = std::vector<int>(rows * columns);

  for (int i = 0; i < points.size(); i++) {
    const auto p = points[i];

    if (p == 0)
      continue;

    if (can_move(i)) {
      next_grid[i + columns] = p;
      continue;
    }

    int left_block = i + columns + -1;
    int right_block = i + columns + 1;

    if (left_block < points.size() && points[left_block] == 0) {
      next_grid[left_block] = p;
    } else if (right_block < points.size() && points[right_block] == 0) {
      next_grid[right_block] = p;
    } else {
      next_grid[i] = p;
    }
  }

  points = next_grid;
}

void Grid::handle_mouse(int hue) {
  int mouse_x = 0;
  int mouse_y = 0;
  SDL_GetMouseState(&mouse_x, &mouse_y);

  auto [grid_x, grid_y] = window_to_grid_coords(mouse_x, mouse_y);

  int boundry = std::floor(DROP_SIZE / 2);
  for (int i = -boundry; i < DROP_SIZE - boundry; i++) {
    for (int j = -boundry; j < DROP_SIZE - boundry; j++) {
      spawn_at(grid_x - i, grid_y - j, hue);
    }
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
  bool mouse_pressed = false;

  auto grid = new Grid(SH / CELL_DIM, SW / CELL_DIM);

  auto previous = SDL_GetTicks();
  Uint32 lag = 0;
  int hue = 1;

  while (!quit) {
    auto current = SDL_GetTicks();
    auto elapsed = current - previous;
    previous = current;
    lag += elapsed;

    while (SDL_PollEvent(&e) != 0) {
      switch (e.type) {
      case SDL_QUIT:
        quit = true;
        break;
      case SDL_MOUSEBUTTONDOWN:
        mouse_pressed = true;
        break;
      case SDL_MOUSEBUTTONUP:
        mouse_pressed = false;
        break;
      case SDL_KEYDOWN:
        if (e.key.keysym.sym == SDLK_r) {
          grid->clean();
        }
      }
    }

    if (mouse_pressed) {
      grid->handle_mouse(hue);
    }

    while (lag >= SCREEN_TICK_PER_FRAME) {
      grid->update();
      lag -= SCREEN_TICK_PER_FRAME;
    }

    grid->render();

    hue += 1;
    if (hue > 360) {
      hue = 1;
    }

    Uint32 frameTime = SDL_GetTicks() - current;
    if (frameTime < SCREEN_TICK_PER_FRAME) {
      SDL_Delay(SCREEN_TICK_PER_FRAME - frameTime);
    }
  }

  close();
  return 0;
}
