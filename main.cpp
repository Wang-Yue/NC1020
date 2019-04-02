#include <SDL2/SDL.h>
#include "nc1020.h"
#include <iostream>

#define FRAME_RATE 30
#define FRAME_INTERVAL (1000/FRAME_RATE)

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 80
#define LINE_SIZE 2

SDL_Renderer* renderer;

static uint8_t lcd_buf[SCREEN_WIDTH * SCREEN_HEIGHT / 8];

bool InitEverything() {
  if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
    std::cout << " Failed to initialize SDL : " << SDL_GetError() << std::endl;
    return false;
  }
  SDL_Window* window =
    SDL_CreateWindow("WQX", 0, 40, LINE_SIZE * SCREEN_WIDTH, LINE_SIZE * SCREEN_HEIGHT, 0);
  if (!window) {
    std::cout << "Failed to create window : " << SDL_GetError() << std::endl;
    return false;
  }
  renderer = SDL_CreateRenderer(window, -1, 0);
  if (!renderer) {
    std::cout << "Failed to create renderer : " << SDL_GetError() << std::endl;
    return false;
  }
  SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH * LINE_SIZE, SCREEN_HEIGHT * LINE_SIZE);

  wqx::WqxRom rom = {
    .romPath = "./obj_lu.bin",
    .norFlashPath = "./nc1020.fls",
    .statesPath = "./nc1020.sts",
  };
  wqx::Initialize(rom);
  wqx::LoadNC1020();
  
  return true;
}

void Render() {
  SDL_RenderClear(renderer);
  SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
    SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

  unsigned char* bytes = nullptr;
  int pitch = 0;
  static const SDL_Rect source = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
  SDL_LockTexture(texture, &source, reinterpret_cast<void**>(&bytes), &pitch);
  static const unsigned char on_color[4] = { 255, 0, 0, 0 };
  static const unsigned char off_color[4] = { 255, 255, 255, 255 };
  static const size_t color_size = sizeof(on_color);
  for (int i = 0; i < sizeof(lcd_buf); ++i) {
    for (int j = 0; j < 8; ++j) {
      bool pixel = (lcd_buf[i] & (1 << (7 - j))) != 0;
      memcpy(bytes, pixel ? on_color : off_color, color_size);
      bytes += color_size;
    }
  }
  SDL_UnlockTexture(texture);
  static const SDL_Rect destination =
    { 0, 0, SCREEN_WIDTH * LINE_SIZE, SCREEN_HEIGHT * LINE_SIZE };
  SDL_RenderCopy(renderer, texture, &source, &destination);
  SDL_RenderPresent(renderer);
  SDL_DestroyTexture(texture);
}

void RunGame() {
  bool loop = true;

  while (loop) {
    uint32_t tick = SDL_GetTicks();

    wqx::RunTimeSlice(FRAME_INTERVAL, false);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if ( event.type == SDL_QUIT ) {
        loop = false;
      } else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
        bool key_down = event.type == SDL_KEYDOWN;
        switch ( event.key.keysym.sym ) {
          case SDLK_RIGHT: // Right
            wqx::SetKey(0x1F, key_down);
            break;
          case SDLK_LEFT:  // Left
            wqx::SetKey(0x3F, key_down);
            break;
          case SDLK_DOWN:  // Down
            wqx::SetKey(0x1B, key_down);
            break;
          case SDLK_UP:  // Up
            wqx::SetKey(0x1A, key_down);
            break;
          case SDLK_u:  // X
            wqx::SetKey(0x10, key_down); // F1 - fly
            break;
          case SDLK_i:  // Y
            wqx::SetKey(0x13, key_down); // F4 - menu
            break;
          case SDLK_j:  // A
            wqx::SetKey(0x1D, key_down); // enter
            break;
          case SDLK_k:  // B
            wqx::SetKey(0x3B, key_down); // ESC - back
            break;
          case SDLK_KP_ENTER:  // START
            wqx::SetKey(0x08, key_down); // F10 - main screen
            break;
          case SDLK_SPACE:  // SELECT
            wqx::SetKey(0x0E, key_down); // F11 - game menu
            break;
          case SDLK_y:  // Shift-X
            wqx::SetKey(0x31, key_down); // X
            break;
          case SDLK_0:  // Shift-Y
            wqx::SetKey(0x25, key_down); // Y
            break;
          case SDLK_h:  // Shift-A
            wqx::SetKey(0x37, key_down); // Page Up
            break;
          case SDLK_l:  // Shift-B
            wqx::SetKey(0x1E, key_down); // Page Down
            break;
          case SDLK_ESCAPE:  // Menu
            wqx::SetKey(0x0F, key_down); // Power
            break;
          case SDLK_BACKSPACE:  // Shift-Menu
            loop = false;
            break;
          default :  // unsupported
            break;
        }
      }
    }
    if (!wqx::CopyLcdBuffer(lcd_buf)) {
      std::cout << "Failed to copy buffer renderer." << std::endl;
    }
    Render();
    tick = SDL_GetTicks() - tick;
    SDL_Delay(FRAME_INTERVAL < tick ? 0 : FRAME_INTERVAL - tick);
  }
}

int main(int argc, char* args[]) {
  if (!InitEverything())
    return -1;
  
  RunGame();
  wqx::SaveNC1020();

  return 0;
}
